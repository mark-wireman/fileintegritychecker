
echo "Adding the GPG key for Grafan to the trusted keys store"
wget -q -O - https://packages.grafana.com/gpg.key | sudo apt-key add -

echo "Adding the Grafan repository and refresh"
sudo add-apt-repository "deb https://packages.grafana.com/oss/deb stable main"
sudo apt update

echo "Install Grafana from the repository"
sudo apt install grafana

echo "Starting the Grafana service and verifying the status"
sudo systemctl enable grafana-server
sudo systemctl start grafana-server
sudo systemctl status grafana-server

