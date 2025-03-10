#!/bin/bash

# Exit on errors
set -e

echo "🚀 Installing dependencies..."
sudo apt update && sudo apt install -y curl g++ libcurl4-openssl-dev libjsoncpp-dev alsa-utils wget git

echo "🔄 Cloning chatbot project..."
git clone https://github.com/jspence394/AI-robot.git ~/AI-robot || (cd ~/AI-robot && git pull)

echo "🎙️ Installing Piper..."
mkdir -p ~/.local/bin
wget https://github.com/rhasspy/piper/releases/latest/download/piper-linux-x86_64.tar.gz -O ~/piper.tar.gz
tar -xvf ~/piper.tar.gz -C ~/.local/bin
rm ~/piper.tar.gz

echo "🎤 Downloading Piper voice model (Danny - en_US)..."
mkdir -p ~/.local/share/piper/en_US-danny
wget https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/danny/low/en_US-danny-low.onnx -O ~/.local/share/piper/en_US-danny/en_US-danny-low.onnx

echo "🛠️ Compiling chatbot..."
cd ~/AI-robot
g++ chatbot.cpp -o chatbot -lcurl -ljsoncpp

echo "📂 Moving chatbot to /usr/local/bin..."
sudo mv chatbot /usr/local/bin/chatbot
sudo chmod +x /usr/local/bin/chatbot

echo "✅ Installation complete! Type 'chatbot' to start chatting."
