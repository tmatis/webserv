## a noter

- un header de `request` contient forcement la commande et `Host:`
sinon il repondra avec le header bad request
- si la commande est mal formater il repondra avec le header bad request
- si le path ne commence pas par /
```
HTTP/1.1 400 Bad Request
Server: nginx/1.14.2
Date: Sat, 09 Oct 2021 10:23:59 GMT
Content-Type: text/html
Content-Length: XXX
Connection: close
```

- si le content length est plus grand on bufferise ce qui reste et on le parse
- on peut avoir plusieur request dans le meme read

- Si on utilise une methode non reconnu on repond avec `405`


```
HTTP/1.1 405 Not Allowed
Server: nginx/1.14.2
Date: Sat, 09 Oct 2021 10:35:29 GMT
Content-Type: text/html
Content-Length: 173
Connection: keep-alive
```

- quand le serveur attend trop longtemps la requete: il close
