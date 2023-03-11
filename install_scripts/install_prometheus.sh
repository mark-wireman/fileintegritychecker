
echo "Setting up group and user 'prometheus'"
sudo groupadd --system prometheus
sudo useradd -s /sbin/nologin --system -g prometheus prometheus

echo "Creating the data and configs directory"
sudo mkdir /var/lib/prometheus
for i in rules rules.d files_sd; do sudo mkdir -p /etc/prometheus/${i}; done

echo "Installing prequisites"
sudo apt update
sudo apt -y install wget curl vim

echo "Downloading the latest Prometheus binary"
kdir -p /tmp/prometheus && cd /tmp/prometheus
curl -s https://api.github.com/repos/prometheus/prometheus/releases/latest | grep browser_download_url | grep linux-amd64 | cut -d '"' -f 4 | wget -qi -

echo "Extracting the contents and moving to the '/usr/local/bin' directory"
tar xvf prometheus*.tar.gz
cd prometheus*/
sudo mv prometheus promtool /usr/local/bin/

echo "Checking the version to confirm installation was successful"
prometheus --version
promtool --version

echo "Moving the prometheus.yml, consoles, and libraries files to '/etc/prometheus'"
sudo mv prometheus.yml /etc/prometheus/prometheus.yml

echo "Creating the service file"

sudo tee /etc/systemd/system/prometheus.service<<EOF
[Unit]
Description=Prometheus
Documentation=https://prometheus.io/docs/introduction/overview/
Wants=network-online.target
After=network-online.target

[Service]
Type=simple
User=prometheus
Group=prometheus
ExecReload=/bin/kill -HUP \$MAINPID
ExecStart=/usr/local/bin/prometheus \
  --config.file=/etc/prometheus/prometheus.yml \
  --storage.tsdb.path=/var/lib/prometheus \
  --web.console.templates=/etc/prometheus/consoles \
  --web.console.libraries=/etc/prometheus/console_libraries \
  --web.listen-address=0.0.0.0:9090 \
  --web.external-url=

SyslogIdentifier=prometheus
Restart=always

[Install]
WantedBy=multi-user.target
EOF

echo "Changing the permission"
for i in rules rules.d files_sd; do sudo chown -R prometheus:prometheus /etc/prometheus/${i}; done
for i in rules rules.d files_sd; do sudo chmod -R 775 /etc/prometheus/${i}; done
sudo chown -R prometheus:prometheus /var/lib/prometheus/

echo "Reloading the daemon and starting the Prometheus service"
sudo systemctl daemon-reload
sudo systemctl start prometheus
sudo systemctl enable prometheus

echo "Confirming the service started successfully"
systemctl status prometheus
