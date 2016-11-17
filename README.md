# console-tcp-listener
Simple one-shot TCP listener for OAuth callback handling  

###Usage

Listen to loopback http://localhost:8080 for 30 seconds

```
tcp-listener
```

Listen to loopback http://localhost:9090 for 60 seconds

```
tcp-listener -p 9090 -t 60
```

Listen to http://localhost:9090 from any address for 90 seconds

```
tcp-listener -p 9090 -t 90 -a
```

```
 -p port: tcp port number to listen (default=8080)
 -t timeout: time to keep listening in secounds (default=30)
 -- : use stdin as tcp response
 -a : accept connection from any address (default=loopback only)
```

From 4D you can pass the ``-`` option and the resonse HTTP (header and body)  in ``StdIn``. The request HTTP (header and body) will be returned ``stdOut``.
