# IPEQ: Querying Multi-Attribute Records with Inner Product Encryption

## Support Queries
- Query 0 (q0)
  - `SELECT count(*) FROM orders WHERE orderstatus = "O"`
- Query 1 (q1)
  - `SELECT count(*) FROM orders WHERE orderstatus = "O" AND orderpriority = "1-URGENT"`
- Query 2 (q2)
  - `SELECT orderstatus, orderpriority, count(orderstatus) FROM orders GROUP BY orderstatus, orderpriority`
- Query 3 (q3)
  - `SELECT count(*) FROM orders JOIN customer ON orders.custkey =  customer.custkey`

## Requirements
  The requirements have to be installed manually (via package manager or building the source code).
  - Docker Compose

## Building IPEQ

We provide a simple Docker build for trying out the library without worrying about the installation and the dependencies. You can build a Docker image yourself by running (possibly with sudo)

```
$ cd ipeq/config/cifer
$ docker-compose build
$ docker-compose up -d
$ docker-compose exec cifer bash
```

In a running container, you will need to set parameters for scale and query.

```
$ cd ipeq/src/fhipe
$ mkdir build && cd build
$ cmake ..
$ make
$ ./main {001/005/01} {q0/q1/q2/q3}
```

## Building homomorphically encrypted DB

```
$ cd ipeq/config/seal
$ docker-compose build
$ docker-compose up -d
$ docker-compose exec seal bash
```

In a running container, you will need to set patameters for scale.
In this scheme we only support q0.

```
$ cd ipeq/src/seal
$ mkdir build && cd build
$ cmake ..
$ make
$ ./main {001/005/01}
```
