# script para criar duas maquinas virtuais
# executar no terminal ./script.sh

sudo ip link add veth0 type veth peer name veth1
sudo ip link set veth0 up
sudo ip link set veth1 up