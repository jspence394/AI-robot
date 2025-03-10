#include <iostream>
#include <string>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <cstdlib> // For system() call
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>

const std::string API_KEY = "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";
const std::string API_URL = "https://api.openai.com/v1/chat/completions";

// Function to handle CURL response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to define the robot's personality
std::string getRobotPersonality() {
    return ""; // Set personality description here
}

// Function to interact with OpenAI Chat API
std::string chatWithAI(const std::string& user_input) {
    CURL* curl;
    CURLcode res;
    std::string response_string;
    std::string read_buffer;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + API_KEY).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        Json::Value jsonRequest;
        jsonRequest["model"] = "gpt-4o";
        jsonRequest["messages"][0]["role"] = "system";
        jsonRequest["messages"][0]["content"] = getRobotPersonality(); // Sends personality

        jsonRequest["messages"][1]["role"] = "user";
        jsonRequest["messages"][1]["content"] = user_input;

        
        Json::StreamWriterBuilder writer;
        std::string jsonData = Json::writeString(writer, jsonRequest);
        
        curl_easy_setopt(curl, CURLOPT_URL, API_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL request failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    // Parse JSON response
    Json::CharReaderBuilder reader;
    Json::Value jsonResponse;
    std::string errs;
    std::istringstream iss(read_buffer);
    
    if (Json::parseFromStream(reader, iss, &jsonResponse, &errs)) {
        return jsonResponse["choices"][0]["message"]["content"].asString();
    } else {
        return "Error parsing response.";
    }
}

// Function to convert text to speech using Piper
void textToSpeech(const std::string& text) {
    std::istringstream iss(text);
    std::vector<std::string> sentences;
    std::string sentence;
    int count = 0;

    // Split text by sentences (assuming '.' as a delimiter)
    while (std::getline(iss, sentence, '.')) {
        if (!sentence.empty()) {
            sentence += '.';  // Add back delimiter
            sentences.push_back(sentence);
        }
    }

    // Process each sentence separately
    std::vector<std::string> filenames;
    for (const auto& s : sentences) {
        std::string filename = "sentence_" + std::to_string(count) + ".wav";
        std::string command = "echo \"" + s + "\" | piper --model ~/.local/share/piper/en_US-danny/en_US-danny-low.onnx --output-file " + filename;
        system(command.c_str());
        filenames.push_back(filename);
        count++;
    }

    // Play each generated audio file sequentially
    for (const auto& filename : filenames) {
        std::string playCommand = "aplay " + filename;
        system(playCommand.c_str());
    }
}

// Main function
int main() {
    while (true) {
        std::string user_input;
        std::cout << "You: ";
        std::getline(std::cin, user_input);
        
        if (user_input == "exit") {
            std::cout << "Exiting chatbot...\n";
            break;
        }

        std::string response = chatWithAI(user_input);
        std::cout << "AI: " << response << std::endl;
        
        textToSpeech(response);
    }
    return 0;
}
