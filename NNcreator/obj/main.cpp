#include <iostream>
#include <windows.h>
#include <fstream>
#include <thread>
#include <conio.h>
#include <iomanip>
#include <random>
#include "NeuralNetwork.h"
#include "Menu.h"
#include "menus.h"

using namespace std;

vector<string> networks{};
NeuralNetwork *currentNeuralNetwork{};
const unsigned int trainSize = 60000, testSize = 10000, imageSize = 784, imageWidth = 28;
double **in{}, **testIn{}, **out{}, **testOut{};
char *prenormalizedOut{}, *prenormalizedTestOut{};

struct {
    string name;
    int layersCount;
    int *layerSize;
    NeuralNetwork::ActivationFunction onOutputLayer;
    NeuralNetwork::ActivationFunction onHiddenLayer;
} futureNeuralNetwork{};

bool loadImages(const string &ubyteFile, double **&images, unsigned int nrOfImages);

bool loadLabels(const string &ubyteFile, char *&labels, unsigned int nrOfImages);

bool writeNetworksToFile();

void normalizeLabels(unsigned int totalCount, char *&prenormalized, double **&normalized, double setCorrectTo, double setWrongTo);

void deallocateData(double **d, unsigned int size);

bool test(double *&x, double *&y, int n);

string activationFunctionTypeToString(NeuralNetwork::ActivationFunction activationFunction);

void activationFunctionBounds(NeuralNetwork::ActivationFunction activationFunction, double &lower, double &upper);

int pickNumber(const string &desc, int from, int to, int lowerBound, int upperBound);

string randomDigit(int digit = -1);

void centerDigit(double *&digit, int x, int y, double background = 0);

int main() {
    ShowWindow(GetConsoleWindow(), SW_MAXIMIZE); // maximize window

    bool databaseLoadedCorrectly{};
    // loading MNIST database in another thread to speed things up
    thread loadDatabase([&] {
        databaseLoadedCorrectly =
                loadImages("train-images.idx3-ubyte", in, trainSize) &&
                loadLabels("train-labels.idx1-ubyte", prenormalizedOut, trainSize) &&
                loadImages("t10k-images.idx3-ubyte", testIn, testSize) &&
                loadLabels("t10k-labels.idx1-ubyte", prenormalizedTestOut, testSize);
    });


    // set menus' dependencies
    preMenu.link(0, menu, false);
    menu.link(0, menu_0);
    for (int i = 0; i < menu_0_0.optionsCount(); i++)
        menu_0_0.link(i, menu_0_0_0, false);
    for (int i = 0; i < menu_0_0_0.optionsCount(); i++)
        menu_0_0_0.link(i, menu_0_0_0_0, false);
    menu_0_0_0_0.link(0, menu_0_1, false);
    menu_0_0_0_0.link(1, menu_0, false); // void from menu_0 0
    menu_0_0_0_0.link(2, menu, false);
    menu_0_1.link(1, menu_0_1_1);
    menu_0_1_1.link(0, menu_0_1_1_0);
    menu_0_1_1_0.link(0, menu_0_1_1_0_0, false);
    menu_0_1_1_0_0.link(0, menu_0_1_1_0, false);
    menu_0_1_1_0_0.link(1, menu_0_1_1, false);
    menu_0_1_1_0.link(1, menu_0_1_1_0_1);
    for (int i = 0; i < menu_0_1_1_0_1.optionsCount() - 1; i++)
        menu_0_1_1_0_1.link(i, menu_0_1_1_0_1_0);
    menu_0_1.link(2, menu_0_1_2);
    menu_0_1.link(3, menu_0, false);
    menu_0_1.link(4, menu, false);
    menu_0_1_2.link(1, menu_0_1_2_1, false);
    menu_0_1_2_1.link(0, menu_0, false);
    menu.link(1, menu_1);
    menu_1.link(0, info[0], true);
    info[4].link(0, menu_1, false);
    for (int i = 0, n = menu_1.optionsCount() - 1; i < n; i++) {
        if (i) menu_1.link(i, info[i], false);
        if (i != n - 1) info[i].link(0, info[i + 1]);
        if (i && i != n - 1) info[i].link(2, menu_1, false);
    }
    menu.link(2, menu_2);
    menu.link(3, menu_3);


    // setting all onClickListeners
    {
        // load neural network from file
        class L_0_1 : public Clickable::OnClickListener {
        private:
            string name;
        public:
            explicit L_0_1(const string &name) {
                L_0_1::name = name;
            }

            void onClick(Clickable &clickable) override {
                try {
                    currentNeuralNetwork = new NeuralNetwork(name); // create object
                    // set its data
                    double down{}, up{};
                    activationFunctionBounds(currentNeuralNetwork->getOutputLayerActivationFunctionType(), down, up);
                    normalizeLabels(trainSize, prenormalizedOut, out, up, down);
                    normalizeLabels(testSize, prenormalizedTestOut, testOut, up, down);

                    currentNeuralNetwork->setTrainData(static_cast<int>(trainSize), in, out);
                    currentNeuralNetwork->setTestData(static_cast<int>(testSize), testIn, testOut);
                    currentNeuralNetwork->setAnswersCheck(test);
                } catch (const ios_base::failure &e) {
                    // neural network's files couldn't be accessed
                    reinterpret_cast<Option &>(clickable).setNextMenu(nullptr);
                    reinterpret_cast<Option &>(clickable).setDescription("ERROR: DATA INACCESSIBLE");
                }
            }
        };

        // set futureNeuralNetwork
        class L_0_0 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                system("cls");

                // set number of layers
                futureNeuralNetwork.layersCount = pickNumber("number of layers", 2, 10, 2, 6);
                futureNeuralNetwork.layerSize = new int[futureNeuralNetwork.layersCount]{};

                // set number of neurons in each layer
                futureNeuralNetwork.layerSize[0] = static_cast<int>(imageSize);
                futureNeuralNetwork.layerSize[futureNeuralNetwork.layersCount - 1] = 10;
                for (int i = 1; i < futureNeuralNetwork.layersCount - 1; i++)
                    futureNeuralNetwork.layerSize[i] =
                            pickNumber("nr of neurons in " + to_string(i) + ". hidden layer",
                                       1, 1000, 10,
                                       // function below sets dynamic suggested upper bound in selection
                                       static_cast<int>(imageSize - (imageSize - 10) *
                                       sqrt(i / (futureNeuralNetwork.layersCount - 1.0))
                                       ));

                if (futureNeuralNetwork.layersCount == 2) {
                    // lack of hidden layers - skip menu with selection of corresponding activation funs
                    futureNeuralNetwork.onHiddenLayer = NeuralNetwork::ActivationFunction::LinearFunction;
                    reinterpret_cast<Option &>(clickable).setNextMenu(&menu_0_0_0);
                } else reinterpret_cast<Option &>(clickable).setNextMenu(&menu_0_0);
            }
        };
        menu_0.setOnClickListener(0, new L_0_0);

        // set futureNeuralNetwork activation functions
        class L_0_0_0 : public Clickable::OnClickListener {
        private:
            bool onOutputLayer;
            NeuralNetwork::ActivationFunction activationFunction;
        public:
            explicit L_0_0_0(NeuralNetwork::ActivationFunction activationFunction, bool output) {
                L_0_0_0::activationFunction = activationFunction;
                onOutputLayer = output;
            }

            void onClick(Clickable &clickable) override {
                (onOutputLayer
                 ? futureNeuralNetwork.onOutputLayer
                 : futureNeuralNetwork.onHiddenLayer)
                        = activationFunction;
            }
        };
        for (int i = 0; i < menu_0_0.optionsCount(); i++)
            menu_0_0.setOnClickListener(i, new L_0_0_0(static_cast<NeuralNetwork::ActivationFunction>(i), false));
        for (int i = 0; i < menu_0_0_0.optionsCount(); i++)
            menu_0_0_0.setOnClickListener(i, new L_0_0_0(static_cast<NeuralNetwork::ActivationFunction>(7 - i), true));

        // create new neural network
        class L_0_0_0_0_0 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                // get date in correct format
                char dateBuffer[12];
                time_t time = chrono::system_clock::to_time_t(chrono::system_clock::now());
                strftime(dateBuffer, 12, "_%Y-%m-%d", localtime(&time));
                futureNeuralNetwork.name =
                        to_string(networks.empty() ? 1 : networks[networks.size() - 2][0] - '0' + 1) + dateBuffer;

                // set neural network's data
                double down{}, up{};
                activationFunctionBounds(futureNeuralNetwork.onOutputLayer, down, up);
                normalizeLabels(trainSize, prenormalizedOut, out, up, down);
                normalizeLabels(testSize, prenormalizedTestOut, testOut, up, down);
                currentNeuralNetwork = new NeuralNetwork(futureNeuralNetwork.name, static_cast<int>(trainSize), in, out,
                                                         futureNeuralNetwork.layersCount, futureNeuralNetwork.layerSize,
                                                         futureNeuralNetwork.onOutputLayer,
                                                         futureNeuralNetwork.onHiddenLayer,
                                                         test);
                currentNeuralNetwork->setTestData(static_cast<int>(testSize), testIn, testOut);

                // add new neural network to menu_0 and networks vector
                networks.push_back(futureNeuralNetwork.name);
                string description = "hidden layers: ";
                if (futureNeuralNetwork.layersCount == 2) {
                    description.append("none");
                } else {
                    for (int i = 1; i < futureNeuralNetwork.layersCount - 1; i++) {
                        if (i != 1) description.append("|");
                        description.append(to_string(futureNeuralNetwork.layerSize[i]));
                    }
                }
                description.append("; ");
                if (futureNeuralNetwork.layersCount > 2) {
                    description.append(activationFunctionTypeToString(futureNeuralNetwork.onHiddenLayer));
                    description.append(" on hidden layer");
                    if (futureNeuralNetwork.layersCount > 3)
                        description.append("s");
                    description.append(" & ");
                }
                description.append(activationFunctionTypeToString(futureNeuralNetwork.onOutputLayer));
                description.append(" on output layer");

                networks.push_back(description);
                writeNetworksToFile();

                menu_0.addOption(Option(futureNeuralNetwork.name, description), 1, true);
                menu_0.setOnClickListener(1, new L_0_1(futureNeuralNetwork.name));
                menu_0.link(1, menu_0_1);
            }
        };
        menu_0_0_0_0.setOnClickListener(0, new L_0_0_0_0_0);

        // some dereferencing
        class L_0_0_0_0_1 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                delete[] futureNeuralNetwork.layerSize;
                futureNeuralNetwork.layerSize = nullptr;
            }
        };
        menu_0_0_0_0.setOnClickListener(1, new L_0_0_0_0_1);
        menu_0_0_0_0.setOnClickListener(2, new L_0_0_0_0_1);

        // learning
        class L_0_1_0 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                currentNeuralNetwork->learn(100);
                currentNeuralNetwork->reload(); // quitting without saving wouldn't work
            }
        };
        menu_0_1.setOnClickListener(0, new L_0_1_0);

        // test neural network in ms paint
        class L_0_1_1_0_0 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                fstream file;
                unsigned int size{};
                unsigned char *bmp{};
                Menu *nextMenu = reinterpret_cast<Option &>(clickable).getNextMenu();
                nextMenu->setDescription("File my_digit.bmp could not be open."); // default

                // load bytes of empty BMP image to array
                file.open("blanc.bin", ios::in | ios::binary | ios::ate);
                if (file.is_open()) {
                    size = file.tellg();
                    file.seekg(0, ios::beg);
                    file.read(reinterpret_cast<char *>(bmp = new unsigned char[size]), size);
                    file.close();
                } else {
                    nextMenu->setDescription("File blanc.bin could not be open.");
                    return;
                }

                // write previously saved bytes to new file
                file.open("my_digit.bmp", ios::out | ios::binary | ios::trunc);
                if (file.is_open()) {
                    file.write(reinterpret_cast<char *>(bmp), size);
                    delete[] bmp;
                    file.close();
                } else {
                    delete[] bmp;
                    return;
                }

                system("mspaint my_digit.bmp"); // open it in ms paint

                int skip = 54;
                // once window is closed load first 54 bytes of user's image to array
                file.open("my_digit.bmp", ios::in | ios::binary | ios::ate);
                if (file.is_open() && file.tellg() > skip) {
                    file.seekg(0, ios::beg);
                    file.read(reinterpret_cast<char *>(bmp = new unsigned char[skip]), skip);
                    file.close();
                } else {
                    file.close();
                    return;
                }

                // check if image isn't corrupted and has unchanged size
                if (bmp[0] != 'B' || bmp[1] != 'M') {
                    nextMenu->setDescription("It's not a BMP file.\n Data may be corrupted.");
                } else if (bmp[28] != 24 || (bmp[10] | bmp[11] << 8 | bmp[12] << 16 | bmp[13] << 24) != skip) {
                    nextMenu->setDescription("Please save image as 24-bit BMP file.\n Unfortunately others won't do.");
                } else if (bmp[30] || bmp[31] || bmp[32] || bmp[33]) {
                    nextMenu->setDescription("Image is compressed and cannot be processed.");
                } else if ((bmp[2] | bmp[3] << 8 | bmp[4] << 16 | bmp[5] << 24) != (102 | 9 << 8) ||
                           (bmp[18] | bmp[19] << 8 | bmp[20] << 16 | bmp[21] << 24) != imageWidth ||
                           (bmp[22] | bmp[23] << 8 | bmp[24] << 16 | bmp[25] << 24) != imageWidth) {
                    nextMenu->setDescription("Please do NOT resize the image!\n Zoom it up instead.");
                } else {
                    // load pixels info
                    file.open("my_digit.bmp", ios::in | ios::binary);
                    if (file.is_open()) {
                        delete[] bmp;
                        file.seekg(skip, ios::beg);
                        file.read(reinterpret_cast<char *>(bmp = new unsigned char[size - skip]), size - skip);
                        file.close();


                        auto input = new double[imageSize]{};
                        // normalize bitmap format for neural network
                        for (int i = imageWidth - 1; i >= 0; i--)
                            for (int j = 0; j < imageWidth; j++)
                                input[(imageWidth - i - 1) * imageWidth + j] =
                                        (255 - bmp[(i * imageWidth + j) * 3]) / 255.0;

                        centerDigit(input, imageWidth, imageWidth); // cenetring with a center of mass
                        auto output = currentNeuralNetwork->calculate(input); // NN's predictions

                        double down{}, up{};
                        activationFunctionBounds(currentNeuralNetwork->getOutputLayerActivationFunctionType(),
                                                 down, up);
                        // find 3 best choices of NN
                        int g[]{-1, -1, -1};
                        for (int i = 0; i < 10; i++) {
                            if (g[2] == -1 || output[i] > output[g[2]]) {
                                g[2] = i;
                                if (g[1] == -1 || output[i] > output[g[1]]) {
                                    g[2] = g[1];
                                    g[1] = i;
                                    if (g[0] == -1 || output[i] > output[g[0]]) {
                                        g[1] = g[0];
                                        g[0] = i;
                                    }
                                }
                            }
                        }
                        char *description = new char[130 + 3 * 16]{};
                        // print outcomes as % chance
                        for (int i = 0, len = 0, guess; i < 10; i++) {
                            auto percentage = (output[i] - down) / (up - down) * 100;
                            guess = (i == g[0]) ? 1 : (i == g[1]) ? 2 : (i == g[2]) ? 3 : 0;

                            sprintf(description + len, "%s%d: %.5s %%%s\n", i ? "  " : " ", i,
                                    to_string(percentage).c_str(), guess ? ((percentage < 0.001)
                                    ? "                " : "   <-- " + to_string(guess)
                                    + ((guess == 1) ? "st" : (guess == 2) ? "nd" : "rd") + " guess").c_str() : "");

                            len += (guess ? 28 : 12) + static_cast<bool>(i);
                        }
                        nextMenu->setDescription(description);
                        delete[] description;
                        delete[] input;
                    }
                }

                delete[] bmp;
            }
        };
        menu_0_1_1_0.setOnClickListener(0, new L_0_1_1_0_0);

        // show random digit
        class L_0_1_1_0_1_0 : public Clickable::OnClickListener {
            int digit;
        public:
            explicit L_0_1_1_0_1_0(int digit = -1) {
                L_0_1_1_0_1_0::digit = digit;
            }

            void onClick(Clickable &clickable) override {
                reinterpret_cast<Option &>(clickable).getNextMenu()->setDescription(randomDigit(digit));
            }
        };
        for (int i = 0; i < menu_0_1_1_0_1.optionsCount() - 1; i++)
            menu_0_1_1_0_1.setOnClickListener(i, new L_0_1_1_0_1_0(i - 1));

        // standard test of neural network
        class L_0_1_1_1 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                currentNeuralNetwork->test();
            }
        };
        menu_0_1_1.setOnClickListener(1, new L_0_1_1_1);

        //delete neural network permanently
        class L_0_1_2_1 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                int index = -1;
                for (int i = 0; i < networks.size(); i += 2) {
                    if (networks[i] == (currentNeuralNetwork->getName())) {
                        index = i;
                        break;
                    }
                }

                if (index != -1) {
                    menu_0.removeOption(menu_0.optionsCount() - index / 2 - 2);

                    networks.erase(networks.cbegin() + index);
                    networks.erase(networks.cbegin() + index);
                    writeNetworksToFile();
                }

                if (NeuralNetwork::deletePermanently(currentNeuralNetwork->getName()) != -1) {
                    reinterpret_cast<Option &>(clickable).getNextMenu()->setDescription(
                            "Neural network deleted correctly."
                    );
                } else {
                    reinterpret_cast<Option &>(clickable).getNextMenu()->setDescription(
                            "Deletion process failed.\n"
                            " You can safely delete corresponding files manually."
                    );
                }

                delete currentNeuralNetwork;
                currentNeuralNetwork = nullptr;
                delete futureNeuralNetwork.layerSize;
                futureNeuralNetwork.layerSize = nullptr;

            }
        };
        menu_0_1_2.setOnClickListener(1, new L_0_1_2_1);

        // dereference neural network
        class L_0_1_3 : public Clickable::OnClickListener {
            void onClick(Clickable &clickable) override {
                delete currentNeuralNetwork;
                currentNeuralNetwork = nullptr;
                delete[] futureNeuralNetwork.layerSize;
                futureNeuralNetwork.layerSize = nullptr;
            }
        };
        menu_0_1.setOnClickListener(3, new L_0_1_3);
        menu_0_1.setOnClickListener(4, new L_0_1_3);

        // changing selected option in menu_1 depending on latest menu from info[]
        class L_1_0_0 : public Clickable::OnClickListener {
            bool forward;
        public:
            explicit L_1_0_0(bool forward) {
                L_1_0_0::forward = forward;
            }

            void onClick(Clickable &clickable) override {
                menu_1.setCurrentOptionIndex(menu_1.getCurrentOptionIndex() + (forward ? 1 : -1));
            }
        };
        info[0].setOnClickListener(0, new L_1_0_0(true));
        info[4].setOnClickListener(1, new L_1_0_0(false));
        for (int i = 1; i < menu_1.optionsCount() - 2; i++) {
            info[i].setOnClickListener(0, new L_1_0_0(true));
            info[i].setOnClickListener(1, new L_1_0_0(false));
        }


        // load saved neural networks' names & descriptions
        fstream saved("save.txt", ios::in);
        if (saved.is_open()) {
            string line{};
            while (getline(saved, line))
                if (!line.empty())
                    networks.push_back(line);

            saved.close();

            if (!networks.empty() && networks[0][0] == '0') {
                // delete pretrained neural network if processor is big-endian - this NN won't work
                short s = 1; // 0x0001
                if (reinterpret_cast<char *>(&s)[1]) { // should be 0x01 for big-endian processors
                    NeuralNetwork::deletePermanently(networks[0]);
                    networks.erase(networks.cbegin());
                    networks.erase(networks.cbegin());
                    writeNetworksToFile();
                }
            }
        } else menu.removeOption(0); // selection of neural networks menu won't be present

        // set onClickListener to all previously saved neural networks + link them
        for (int i = 0; i < networks.size(); i += 2) {
            menu_0.addOption(Option(networks[i], networks[i + 1]), 1);
            menu_0.setOnClickListener(1, new L_0_1(networks[i]));
            menu_0.link(1, menu_0_1);
        }
    }

    loadDatabase.join(); // main thread will wait for loadDatabase thread until it's finished

    if (databaseLoadedCorrectly) {
        // every file present & happy to cooperate
        menu_3.setOnClickListener(1, Menu::Exit::create());
        preMenu.setDescription(" WELCOME TO THE CREATOR OF YOUR VERY OWN NEURAL NETWORKS!\n" + randomDigit());
    } else {
        // some file(s) absent or not opening
        preMenu.setOnClickListener(0, Menu::Exit::create());
        preMenu.setDescription("             An error occurred while loading\n"
                               "                    THE  MNIST DATABASE\n"
                               "                   of handwritten digits\n");
    }

    // begin navigation through menus
    Menu::navigate(preMenu);


    // deallocation
    delete[] prenormalizedOut;
    delete[] prenormalizedTestOut;

    deallocateData(in, trainSize);
    deallocateData(out, trainSize);
    deallocateData(testIn, testSize);
    deallocateData(testOut, testSize);

    system("pause"); //TODO: remove this
    return 0;
}

// set correct and wrong values in normalized labels data
void normalizeLabels(unsigned int totalCount, char *&prenormalized, double **&normalized, double setCorrectTo,
                     double setWrongTo) {
    deallocateData(normalized, totalCount);
    normalized = new double *[totalCount]{};

    for (int i = 0; i < totalCount; i++) {
        normalized[i] = new double[10]{};
        for (int j = 0; j < 10; j++)
            normalized[i][j] = setWrongTo;
        normalized[i][prenormalized[i]] = setCorrectTo;
    }
}

// this test checks if indexes of the biggest output & desired output match
bool test(double *&x, double *&y, int n) {
    int correct = 0, max = 0;
    for (int i = 1; i < n; i++) {
        if (y[correct] < y[i]) correct = i;
        if (x[max] < x[i]) max = i;
    }
    return correct == max;
}

// save images from MNIST to normalized matrix
bool loadImages(const string &ubyteFile, double **&images, unsigned int nrOfImages) {
    fstream file(ubyteFile, ios::in | ios::binary);
    char *bytes{};

    if (file.is_open()) {
        file.seekg(16, ios::beg);
        file.read(bytes = new char[nrOfImages * imageSize], nrOfImages * imageSize);
        file.close();

        images = new double *[nrOfImages]{};
        for (int i = 0; i < nrOfImages; i++) {
            images[i] = new double[imageSize]{};
            for (int j = 0; j < imageSize; j++)
                images[i][j] = static_cast<unsigned char>(bytes[i * imageSize + j]) / 255.0;
        }

        delete[] bytes;
        return true;
    } else return false;
}

// save labels from MNIST to prenormalized matrix
bool loadLabels(const string &ubyteFile, char *&labels, unsigned int nrOfImages) {
    fstream file(ubyteFile, ios::in | ios::binary);
    if (file.is_open()) {
        file.seekg(8, ios::beg);
        file.read(labels = new char[nrOfImages], nrOfImages);
        file.close();
        return true;
    } else return false;
}

bool writeNetworksToFile() {
    fstream file("save.txt", ios::out | ios::trunc);
    if (file.is_open()) {
        for (auto &str : networks)
            file << str << endl;
        file.close();
        return true;
    } else return false;
}

void deallocateData(double **d, unsigned int size) {
    if (d != nullptr) {
        for (int i = 0; i < size; i++)
            delete[] d[i];
        delete[] d;
    }
}

string activationFunctionTypeToString(NeuralNetwork::ActivationFunction activationFunction) {
    switch (activationFunction) {
        case NeuralNetwork::ActivationFunction::LogisticFunction:
            return "logistic function";
        case NeuralNetwork::ActivationFunction::ReLU:
            return "ReLU";
        case NeuralNetwork::ActivationFunction::LeakyReLU:
            return "leaky ReLU";
        case NeuralNetwork::ActivationFunction::LinearFunction:
            return "linear function";
        case NeuralNetwork::ActivationFunction::Tanh:
            return "hyperbolic tangent";
        case NeuralNetwork::ActivationFunction::ArcTan:
            return "arcus tangent";
        case NeuralNetwork::ActivationFunction::ReLU6:
            return "ReLU-6";
        case NeuralNetwork::ActivationFunction::ELU:
            return "ELU";
    }

    return "default";
}

// changes lower & upper bounds depending on activation function
void activationFunctionBounds(NeuralNetwork::ActivationFunction activationFunction, double &lower, double &upper) {
    lower = 0.0, upper = 1.0;
    switch (activationFunction) {
        case NeuralNetwork::ActivationFunction::Tanh:
            lower = -1.0;
            break;
        case NeuralNetwork::ActivationFunction::ArcTan:
            upper = 3.1415926535897932384626433832795028841971694 / 2;
            lower = -upper;
            break;
        case NeuralNetwork::ActivationFunction::ReLU6:
            upper = 6.0;
            break;
        default:;
    }
}

// let user pick some number from 'from' to 'to' with suggested lower & upper bounds
int pickNumber(const string &desc, int from, int to, int lowerBound, int upperBound) {
    // this function is definitely overcomplicated for what it offers (but works great)
    string DESC = static_cast<char>(desc[0] - 'a' + 'A') + desc.substr(1); // desc but big

    // -1 - default state
    //  0 - value < from
    //  1 - value < lowerBound
    //  2 = value > upperBound
    char state = -1;
    int value = to, buffer = -1, i = 0, digitCount = 0;
    do {
        digitCount++; // number of digits in max value
        value /= 10;
    } while (value);

    bool enterNotHit = true, reload = true, fix = false;
    cout << setfill('0');
    do {
        if (reload) {
            cout << endl << ' ';

            switch (state) {
                case 0:
                    cout << DESC << " must be equal to at least " << from << '.';
                    break;
                case 1:
                    cout << DESC << " may be to low.";
                    break;
                case 2:
                    cout << DESC << " may be to large.";
                    break;
                default:
                    if (i) cout << "Hit /ENTER/ to accept " << desc << '.'; // fine value
                    else cout << "Enter " << desc << ':'; // nothing entered
            }
            cout << endl << ' ';
            if (i) cout << setw(i + fix) << value;
        }
        reload = true;

        int input = _getch();
        if (input >= '0' && input <= '9') { // digit entered
            input = input - '0';

            if (value == to) { // current value = max value
                // reset input value and set it to this digit
                buffer = -1;
                value = input;
                i = 1;
                fix = false;
            } else if (value * 10 + input > to) { // value greater than max value
                // so set it to max value
                buffer = value;
                int n = 0;
                do {
                    n++;
                    buffer /= 10;
                } while (buffer);
                fix = n < digitCount; // fix is cosmetically important

                buffer = value; // remember previous value in case of backspace
                value = to;
            } else { // value smaller than max value - nothing interesting happens
                (value *= 10) += input;
                cout << input;
                i++; // keep track of amount of entered digits - helps with setw
            }
        } else if (input == '\b') { // backspace hit
            if (buffer != -1) { // get previous value
                value = buffer;
                buffer = -1;
                fix = false;
            } else { // just delete one digit
                value /= 10;
                if (i) i--;
            }
        } else if (input == 13 && i && value >= from) { // enter
            enterNotHit = false;
        } else {
            reload = false;
        }

        // change state
        if (i) {
            if (value < from) {
                state = 0;
            } else if (value < lowerBound) {
                state = 1;
            } else if (value > upperBound) {
                state = 2;
            } else {
                state = -1;
            }
        } else state = -1;

        if (reload) system("cls");
    } while (enterNotHit);

    cout << setfill(' ');
    return value;
}

// creates an image of a random digit with cool shades & everything
string randomDigit(int d) {
    string digit{};
    char shades[]{' ', -80, -79, -78, -37};

    random_device device;
    mt19937_64 mt(device());
    uniform_int_distribution<int> distribution(0, trainSize - 1);
    unsigned int offset = distribution(mt);

    if (d > -1 && d < 10)
        // find correct digit from MNIST if yet not found
        while (prenormalizedOut[offset] != d)
            if (++offset == trainSize) offset = 0;

    double *pixels = *(in + offset);

    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageWidth; j++) {
            auto pixel = static_cast<unsigned char>(pixels[i * imageWidth + j] * 255);
            pixel = (pixel < 10)
                    ? 0 : (pixel < 85)
                          ? 1 : (pixel < 165)
                                ? 2 : (pixel < 245)
                                      ? 3 : 4; // pick shade depending on value of pixel

            (digit += shades[pixel]) += shades[pixel]; // append twice
        }
        digit += '\n';
    }
    return digit;
}

// centers a digit saved as array by calculating its center of mass of pixels
void centerDigit(double *&digit, int x, int y, double background) {
    double xCenter = 0, yCenter = 0, massTotal = 0;

    // calculate center of mass
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            auto mass = digit[i * y + j];
            xCenter += mass * i;
            yCenter += mass * j;
            massTotal += mass;
        }
    }

    xCenter /= massTotal;
    yCenter /= massTotal;

    int deltaX = (x - 1) / 2 - static_cast<int>((x % 2) ? round(xCenter) : xCenter);
    int deltaY = (y - 1) / 2 - static_cast<int>((y % 2) ? round(yCenter) : yCenter);

    // move pixels to center the center of mass inside picture
        if (deltaX >= 0) {
        for (int step = 0; step < deltaX; step++) {
            for (int i = x - 2; i >= 0; i--) {
                for (int j = 0; j < y; j++)
                    digit[(i + 1) * y + j] = digit[i * y + j];
            }
            for (int i = 0; i < y; i++)
                digit[i] = background;
        }
    } else {
        for (int step = 0; step < -deltaX; step++) {
            for (int i = 0; i < x - 1; i++) {
                for (int j = 0; j < y; j++)
                    digit[i * y + j] = digit[(i + 1) * y + j];
            }
            for (int i = 0; i < y; i++)
                digit[(x - 1) * y + i] = background;
        }
    }

    if (deltaY >= 0) {
        for (int step = 0; step < deltaY; step++) {
            for (int i = 0; i < x; i++) {
                for (int j = y - 2; j >= 0; j--)
                    digit[i * y + j + 1] = digit[i * y + j];
                digit[i * y] = background;
            }
        }
    } else {
        for (int step = 0; step < -deltaY; step ++) {
            for (int i = 0; i < x; i++) {
                for (int j = 0; j < y - 1; j++)
                    digit[i * y + j] = digit[i * y + j + 1];
                digit[(i + 1) * y - 1] = background;
            }
        }
    }
}