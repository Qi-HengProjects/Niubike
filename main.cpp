#include <iostream>
#include <string>
#include <sstream>

using namespace std;

string getCenteredString(const string& text, int width = 80)
{
    int padding = (width - static_cast<int>(text.length())) / 2;
    if (padding > 0)
    {
        return string(padding, ' ') + text;
    }
    return text;
}


void printCenteredBlock(const string& text, int width = 80)
{
    stringstream ss(text);
    string line;

    while (getline(ss, line))
    {
        cout << getCenteredString(line, width) << '\n';
    }
}

int login()
{
    const string asciiArt = R"(
 _____  ___    __     ____  ____  _______   __     __   ___  _______
(\"   \|"  \  |" \   ("  _||_ " ||   _  "\ |" \   |/"| /  ")/"     "|
|.\\   \    | ||  |  |   (  ) : |(. |_)  :)||  |  (: |/   /(: ______)
|: \.   \\  | |:  |  (:  |  | . )|:     \/ |:  |  |    __/  \/    |
|.  \    \. | |.  |   \\ \__/ // (|  _  \\ |.  |  (// _  \  // ___)_
|    \    \ | /\  |\  /\\ __ //\ |: |_)  :)/\  |\ |: | \  \(:      "|
 \___|\____\)(__\_|_)(__________)(_______/(__\_|_)(__|  \__)\_______)
)";


    printCenteredBlock(asciiArt, 165);

    string usernameInput, pwdInput;

    cout << endl << endl;
    cout << getCenteredString("Username: ", 145);
    cin >> usernameInput;

    //database input here for username

    cout << getCenteredString("Password: ", 145);
    cin >> pwdInput;

    //database input here for username
    return 0;
}

int main()
{
    login();
    return 0;
}