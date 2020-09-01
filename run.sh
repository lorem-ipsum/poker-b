qmake
make
make clean
./poker-b


# ssh clt-server \
# "rm -rf ~/clt/poker-a \
# && (fuser 10080/tcp && fuser -k 10080/tcp && echo 'KILL!' || echo 'port is already free') \
# && exit"

# scp -r ../poker-a ubuntu@81.70.38.165:/home/ubuntu/clt/

# ssh clt-server \
# "(cd ./clt/dual-server \
# && qmake \
# && make \
# && make clean \
# && ./poker-a)"