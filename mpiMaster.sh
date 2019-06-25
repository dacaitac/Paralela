######### MAESTRO ###############
sudo apt-get update
sudo apt-get upgrade
sudo adduser mpiuser --uid 999
# Se debe agregar mpiuser a /etc/sudoers
# mpiuser ALL=(ALL:ALL) ALL
sudo apt-get install openmpi-bin openmpi-common libopenmpi-dev
sudo apt-get install nfs-kernel-server
sudo apt-get install nfs-common
su mpiuser
ssh-keygen -t rsa

ssh-copy-id mpiuser@instance-1

echo "/home/mpiuser *(rw,sync,no_subtree_check)" > /etc/exports
# Si la linea anterior no funciona se agrega 
# /home/mpiuser *(rw,sync,no_subtree_check)
# al archivo /etc/exports

sudo service nfs-kernel-server restart
sudo exportfs -a

sudo mount -t nfs ip_del_master:/home/mpiuser /home/mpiuser/


######### NODOS ###############
sudo apt-get update
sudo apt-get upgrade
sudo adduser mpiuser --uid 999
# Se debe agregar mpiuser a /etc/sudoers
# mpiuser ALL=(ALL:ALL) ALL
sudo apt-get install openmpi-bin openmpi-common libopenmpi-dev
sudo apt-get install nfs-common

ssh-keygen -t rsa
ssh-copy-id mpiuser@master
    
# Para correr el programa:
# mpirun -np 4 --hostfile mpi-hosts ./mpi_test


# Los errores de ssh se corrigen desde el cliente
# https://www.digitalocean.com/community/questions/error-permission-denied-publickey-when-i-try-to-ssh
    