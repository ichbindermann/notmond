# Notification Monitor
Real-time linux daemon for monitoring system notifications

# Requirements
* gcc
* cmake
* make
* libcurl
* json-c
* systemd

# Building
```
git clone https://github.com/ichbindermann/notmond.git
cd notmond
mkdir build
cmake ..
make
```

# Usage
```./notmond -config=/etc/notmond/config.json -terminal=enable```
- Argument config for read config
- Argument terminal for use interactive mode (available commands 'exit' for exit :) )

# Configuration explanation
```
mkdir /etc/notmond/
vim /etc/notmond/config.json
```
Example config file
```
{
  "interval": "5",
  "line_count": "1",
  "temporary_dir": "/tmp/notmond/",
  "telebot_module":
  {
    "enable": "true",
    "token": "bot6761212352:Oj8JHAGGDS-W23568bfA",
    "proxy": "socks5://127.0.0.1:9050",
    "master": "1231234123",
    "api_url": "https://api.telegram.org/"
  }
}
```
* interval - secs before checking (default: 5)
* line_count - lines for checking (default: 1)
* temporary_dir - for temporary files and data (not used, yet)
* telebot_module - its internal module for sending notification
* telebot_module -> enable - true/false this module
* telebot_module -> token - put your telegram bot token
* telebot_module -> proxy - use proxy for access telegram services (in my country telegram was restricted)
* telebot_master -> master - put your chat_id of telegram for getting notification from your bot
* telebot_module -> api_url - use official server of telegram or mirror (needed for avoid other restrictions)

# Whats working
- Telegram bot sending notification about accapted ssh logins
