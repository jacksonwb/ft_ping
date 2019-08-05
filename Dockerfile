FROM debian

Run apt-get update
Run apt-get install build-essential -y

ADD . /ping
WORKDIR /ping

ENTRYPOINT ["/bin/bash"]