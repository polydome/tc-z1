# Techonologie Chmurowe - zadanie 1
Autor: Łukasz Burzak

Aby zbudować obraz kontenera:
```shell
docker buildx build -t polydome/z1 .
```

Aby uruchomić kontener:
```shell
docker run -p 8080:8080 --rm -d --name lb-z1 polydome/z1
```

Aby odczytać logi serwera:
```shell
docker logs lb-z1
```

Aby sprawdzić ile warstw posiada zbudowany obraz:
```shell
docker history polydome/z1
```

## Dodatek 1
```shell
# Zmienna środowiskowa `REGISTRY_HTTP_ADDR` określa, na jakim adresie usługa nasłuchuje żądań
docker run -d -e REGISTRY_HTTP_ADDR=0.0.0.0:6677 -p 6677:6677 --name reg registry:2
# Nowy tag jest poprzedzony adresem usługi `registry`
docker image tag ubuntu localhost:6677/ubuntu
docker push localhost:6677/ubuntu
```

Wynik działań:
![Wynik działań](assets/d1.png)

## Dodatek 2
Na początku należy utworzyć użytkownika, który będzie mógł dokonać uwierzytelnienia.
```shell
# Tworzy katalog przechowujący dane uwierzytelniające użytkowników
mkdir auth
# Tworzy użytkownika o nazwie "user" i haśle "pass" w pliku `auth/htpasswd`
docker run --entrypoint htpasswd httpd:2 -Bbn user pass > auth/htpasswd
```
![Tworzenie użytkownika](assets/d2_setup_user.png)

Następnie należy uruchomić usługę `registry` z innymi argumentami:
```shell
docker stop reg
docker run -d \
	-p 6677:6677 \
	--restart=always \
	--name reg \
	-e REGISTRY_HTTP_ADDR=0.0.0.0:6677 \
	# Montuje katalog zawierający plik z danymi uwierzytelniającymi
	-v "$(pwd)"/auth:/auth \
	# Określa sposób uwierzytelnienia
	-e "REGISTRY_AUTH=htpasswd" \
	-e "REGISTRY_AUTH_HTPASSWD_REALM=Registry Realm" \
	# Określa lokalizację pliku zawierającego dane uwierzytelniające
	-e REGISTRY_AUTH_HTPASSWD_PATH=/auth/htpasswd \
	registry:2
```

Użycie komendy `push` bez uwierzytelnienia jest niedozwolone:
![Nieudana próba](assets/d2_auth_fail.png)

Po zalogowaniu `push` przebiega pomyślnie:
![Udana próba](assets/d2_auth_success.png)