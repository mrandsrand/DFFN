#include <iostream>
#include <fstream>
#include <thread>
#include <conio.h>
#include <random>
#include <direct.h>
#include <iomanip>
#include "NeuralNetwork.h"

using namespace std;

// construcor for already created networks - doesn't require any NN's properties but name
NeuralNetwork::NeuralNetwork(const std::string &name, double learnRate) noexcept(false) {
    NAME = name;
    path = "neural_networks\\" + name;
    NeuralNetwork::examplesCount = -1;
    NeuralNetwork::in = nullptr;
    NeuralNetwork::out = nullptr;
    isCorrect = nullptr;

    // load data from file
    fstream file(path + "\\data.bin", ios::in | ios::binary);
    if (file.is_open()) {
        unsigned char actFun{};
        file.read(reinterpret_cast<char *>(&layersCount), sizeof(int));
        file.read(reinterpret_cast<char *>(layerSize = new int[layersCount]), sizeof(int) * layersCount);
        file.read(reinterpret_cast<char *>(&actFun), sizeof(char));
        setActivationFunction(outputLayerActivationFunction, static_cast<ActivationFunction>(actFun));
        file.read(reinterpret_cast<char *>(&actFun), sizeof(char));
        setActivationFunction(hiddenLayerActivationFunction, static_cast<ActivationFunction>(actFun));
        file.close();
    } else throw ios_base::failure("data not found");

    if (reload(false)) { // test if is opening
        setData();
        reload(); // load weights and biases
    } else throw ios_base::failure("lacking data");

    if (learnRate > 0) LEARN_RATE = learnRate;
}

// constructor for new networks
NeuralNetwork::NeuralNetwork(const string &name, int examplesCount, double **&in, double **&out,
                             int layersCount, int *layerSize,
                             ActivationFunction outputLayerActivationFunction,
                             ActivationFunction hiddenLayerActivationFunction,
                             bool (*checkAnswers)(double *&calculated, double *&correct, int size), double learnRate) {
    NAME = name;
    path = "neural_networks\\" + name;
    NeuralNetwork::examplesCount = examplesCount;
    NeuralNetwork::in = in;
    NeuralNetwork::out = out;
    isCorrect = checkAnswers;

    NeuralNetwork::layersCount = layersCount;
    NeuralNetwork::layerSize = new int[layersCount];
    for (int i = 0; i < layersCount; i++)
        NeuralNetwork::layerSize[i] = layerSize[i];

    LEARN_RATE = learnRate;

    setData();

    // create path if not present
    _mkdir("neural_networks");
    _mkdir(path.c_str());

    // set random weights and biases
    random_device device;
    mt19937 mt(device());
    uniform_real_distribution<double> weights_distribution(-0.5, 0.5);
    uniform_real_distribution<double> biases_distribution(0, 1);

    for (int i = 0; i < layersCount - 1; i++) {
        for (int j = 0; j < layerSize[i + 1]; j++) {
            for (int k = 0; k < layerSize[i]; k++)
                w[i][k][j] = weights_distribution(mt);
            b[i][j] = biases_distribution(mt);
        }
    }

    setActivationFunction(NeuralNetwork::outputLayerActivationFunction, outputLayerActivationFunction);
    setActivationFunction(NeuralNetwork::hiddenLayerActivationFunction, hiddenLayerActivationFunction);

    fstream file(path + "\\data.bin", ios::out | ios::binary | ios::trunc);
    if (file.is_open()) { // write properties to file so that the second constructor could be used next time
        unsigned char type{};
        file.write(reinterpret_cast<char *>(&layersCount), sizeof(int));
        file.write(reinterpret_cast<char *>(layerSize), sizeof(int) * layersCount);
        type = static_cast<unsigned char>(outputLayerActivationFunction);
        file.write(reinterpret_cast<char *>(&type), sizeof(char));
        type = static_cast<unsigned char>(hiddenLayerActivationFunction);
        file.write(reinterpret_cast<char *>(&type), sizeof(char));
        file.close();
    }
    save(); // save w & b already in case of learn() method not used in program
}

// dynamically create the body of neural network
void NeuralNetwork::setData() {

    a = new double *[layersCount];
    z = new double *[layersCount - 1];
    b = new double *[layersCount - 1];
    w = new double **[layersCount - 1];

    db = new double *[layersCount - 1];
    dw = new double **[layersCount - 1];

    for (int i = 0; i < layersCount - 1; i++) {
        a[i + 1] = new double[layerSize[i + 1]];

        z[i] = new double[layerSize[i + 1]];
        b[i] = new double[layerSize[i + 1]];
        w[i] = new double *[layerSize[i]];
        db[i] = new double[layerSize[i + 1]];
        dw[i] = new double *[layerSize[i]];

        for (int j = 0; j < layerSize[i]; j++) {
            w[i][j] = new double[layerSize[i + 1]];
            dw[i][j] = new double[layerSize[i + 1]];
        }
    }

    hasTestData = false;
    testIn = nullptr;
    testOut = nullptr;
    testExamplesCount = -1;
}

NeuralNetwork::~NeuralNetwork() {
    in = nullptr;
    out = nullptr;
    testIn = nullptr;
    testOut = nullptr;

    a[0] = nullptr;
    for (int i = layersCount - 2; i >= 0; i--) {
        for (int j = layerSize[i] - 1; j >= 0; j--) {
            delete[] dw[i][j];
            delete[] w[i][j];
        }
        delete[] dw[i];
        delete[] db[i];
        delete[] w[i];
        delete[] b[i];
        delete[] z[i];

        delete[] a[i + 1];
    }

    delete[] dw;
    delete[] db;
    delete[] w;
    delete[] b;
    delete[] z;
    delete[] a;

    delete outputLayerActivationFunction;
    delete hiddenLayerActivationFunction;

    delete[] layerSize;
}

void NeuralNetwork::setActivationFunction(BaseActivationFunction *&activationFunction,
                                          NeuralNetwork::ActivationFunction setTo) {
    switch (setTo) {
        case ActivationFunction::LogisticFunction:
            activationFunction = new LogisticFunction(*this);
            break;
        case ActivationFunction::ReLU:
            activationFunction = new ReLU(*this);
            break;
        case ActivationFunction::LeakyReLU:
            activationFunction = new LeakyReLU(*this);
            break;
        case ActivationFunction::LinearFunction:
            activationFunction = new LinearFunction(*this);
            break;
        case ActivationFunction::Tanh:
            activationFunction = new Tanh(*this);
            break;
        case ActivationFunction::ArcTan:
            activationFunction = new ArcTan(*this);
            break;
        case ActivationFunction::ReLU6:
            activationFunction = new ReLU6(*this);
            break;
        case ActivationFunction::ELU:
            activationFunction = new ELU(*this);
            break;
    }
}

void NeuralNetwork::learn(int learningSessionsPerInfo, int lessonsPerLearningSession) {

    if (in == nullptr || out == nullptr || examplesCount == -1) {
        cerr << "Train data not set. Use void NeuralNetwork::setTrainData beforehand" << endl;
        return;
    }

    random_device device;
    mt19937_64 mt(device());
    // random integer corresponding to an example from in
    uniform_int_distribution<int> distribution(0, examplesCount - 1);

    bool isLearning = true, needToSave = false, changeLearnRate = false, timeToTest = false;
    thread stop([&] { // thread to register key strokes from user
        while (isLearning)
            if (!changeLearnRate && !timeToTest)
                switch (_getch()) {
                    case 27: // esc
                        isLearning = false;
                        break;
                    case 0: // arrows
                    case 224:
                        changeLearnRate = true;
                        break;
                    case 't':
                    case 'T':
                        if (isCorrect != nullptr) timeToTest = true;
                        break;
                    case 'q':
                    case 'Q':
                        isLearning = false;
                    case 's':
                    case 'S':
                        needToSave = true;
                        break;
                }
    });

    system("cls");
    cout << "..." << setprecision(4);

    double storeCostInfo; // buffer for a value of previous cost
    unsigned int LESSONS_PER_INFO = lessonsPerLearningSession * learningSessionsPerInfo;

    while (isLearning) {
        double totalCost = 0;
        int learningSession;

        for (learningSession = 0; isLearning && learningSession < learningSessionsPerInfo; learningSession++) {
            for (int i = 0, r; i < lessonsPerLearningSession; i++) {
                r = distribution(mt); // random int
                calculate(in[r]); // calculate random input vector
                totalCost += cost(out[r]); // get cost of this lesson
                backpropagation(out[r]); // backpropagate
            }

            // add negative gradient of the cost function to w & b after a learning session
            for (int i = 0; i < layersCount - 1; i++) {
                for (int j = 0; j < layerSize[i + 1]; j++) {
                    for (int k = 0; k < layerSize[i]; k++)
                        w[i][k][j] -= dw[i][k][j] / lessonsPerLearningSession * LEARN_RATE;
                    b[i][j] -= db[i][j] / lessonsPerLearningSession * LEARN_RATE;
                }
            }

            // user actions
            if (needToSave) {
                save();
                cout << endl << "Saved.";
                needToSave = false;
            }
            if (changeLearnRate) {
                switch (_getch()) {
                    case 72:
                    case 77:
                        LEARN_RATE < 1
                        ? LEARN_RATE *= 2
                        : LEARN_RATE += 1;
                        break;
                    case 75:
                    case 80:
                        LEARN_RATE > 1
                        ? LEARN_RATE -= 1
                        : LEARN_RATE /= 2;
                        break;
                }
                changeLearnRate = false;
                break;
            }
            if (timeToTest) {
                test();
                timeToTest = false;
                break;
            }
        }
        system("cls");
        cout << "Press:" << endl
             << ((isCorrect == nullptr) ? "" : "   /T/ to test\n")
             << "   /S/ to just save" << endl
             << "   /Q/ to save and quit" << endl
             << "   /esc/ to quit without saving" << endl
             << "   /<-/ or /->/ to adjust learn rate" << endl
             << endl << "Average cost based on " << LESSONS_PER_INFO << " lessons: "
             // storeCostInfo is printed if LESSONS_PER_INFO lessons were not conducted
             << ((learningSession < learningSessionsPerInfo) ? storeCostInfo : storeCostInfo =
                                                                                       totalCost / LESSONS_PER_INFO)
             << endl << "Learn rate: " << LEARN_RATE << endl;

    }
    stop.join();
    if (needToSave) save();
}

// a yes/no test with percentage of correct answers and average cost
void NeuralNetwork::simpleTest(const string &testName, int &n, double **&x, double **&y) {
    cout << testName << ':' << endl;

    double totalCost = 0;
    int correct = 0;

    //sum up all correct answers and cost
    for (int i = 0; i < n; i++) {
        calculate(x[i]);
        correct += isCorrect(a[layersCount - 1], y[i], layerSize[layersCount - 1]);
        totalCost += cost(y[i]);
    }
    // print outcomes
    cout << setw(20) << left << "   Correct answers: "
         << 100.0 * correct / n << '%' << endl
         << setw(20) << "   Average cost: "
         << totalCost / n << endl << right;
}

// simpleTest of both training set & test set
void NeuralNetwork::test() {
    system("cls");
    simpleTest("Training set", examplesCount, in, out);
    if (hasTestData)
        simpleTest("Test set", testExamplesCount, testIn, testOut);

    cout << endl << "Press any key to continue." << endl;
    _getch();
}

// an algorithm for calculating a gradient of the cost function
void NeuralNetwork::backpropagation(double *&outputs) {
    BaseActivationFunction *activationFunction = outputLayerActivationFunction;

    auto dCda = new double[layerSize[layersCount - 1]]; // a derivative of cost fun with respect to a
    for (int i = 0; i < layerSize[layersCount - 1]; i++)
        dCda[i] = a[layersCount - 1][i] - outputs[i];


    for (int layer = layersCount - 2; layer >= 0; layer--) {
        double buffer[layerSize[layer]];
        for (int i = 0; i < layerSize[layer]; i++) buffer[i] = 0;

        for (int i = 0; i < layerSize[layer + 1]; i++) {
            db[layer][i] = activationFunction->derivative(layer, i) * dCda[i]; // calculate dC/db

            for (int j = 0; j < layerSize[layer]; j++) {
                dw[layer][j][i] = a[layer][j] * db[layer][i];  // calculate dC/dw
                if (layer) buffer[j] += w[layer][j][i] * db[layer][i]; // calculate dC/da
            }
        }

        delete[] dCda;
        if (layer) {
            dCda = new double[layerSize[layer]]; // save dC/da from previous layer
            for (int i = 0; i < layerSize[layer]; i++) dCda[i] = buffer[i];
            activationFunction = hiddenLayerActivationFunction; // change activation fun after output layer
        }
    }
}

// function changing input vector into NN's predictions
double *NeuralNetwork::calculate(double *&inputs) {
    a[0] = inputs;

    for (int i = 0; i < layersCount - 1; i++) {
        for (int j = 0; j < layerSize[i + 1]; j++) {
            z[i][j] = b[i][j];
            for (int k = 0; k < layerSize[i]; k++)
                z[i][j] += w[i][k][j] * a[i][k];

            a[i + 1][j] = (i == layersCount - 2) // use activation fun on z
                          ? outputLayerActivationFunction->use(z[i][j])
                          : hiddenLayerActivationFunction->use(z[i][j]);
        }
    }
    return a[layersCount - 1];
}

double NeuralNetwork::cost(double *&expected) {
    double costVal = 0;
    for (int i = 0; i < layerSize[layersCount - 1]; i++)
        costVal += pow(a[layersCount - 1][i] - expected[i], 2);

    return 0.5 * costVal;
}

// write weights & biases (also learn rate) to file
bool NeuralNetwork::save() {
    fstream file(path + "\\weights_and_biases.bin", ios::out | ios::binary | ios::trunc);

    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(&LEARN_RATE), sizeof(double));

        for (int i = 0; i < layersCount - 1; i++) {
            unsigned int size = sizeof(double) * layerSize[i + 1];

            for (int j = 0; j < layerSize[i]; j++)
                file.write(reinterpret_cast<char *>(w[i][j]), size);
            file.write(reinterpret_cast<char *>(b[i]), size);
        }

        file.close();
        return true;
    } else return false;
}

// load weights & biases (also learn rate) from file
bool NeuralNetwork::reload(bool openAndRead) {
    fstream file(path + "\\weights_and_biases.bin", ios::in | ios::binary);

    if (file.is_open()) {
        if (openAndRead) {
            file.read(reinterpret_cast<char *>(&LEARN_RATE), sizeof(double));

            for (int i = 0; i < layersCount - 1; i++) {
                unsigned int size = sizeof(double) * layerSize[i + 1];

                for (int j = 0; j < layerSize[i]; j++)
                    file.read(reinterpret_cast<char *>(w[i][j]), size);
                file.read(reinterpret_cast<char *>(b[i]), size);
            }
        }

        file.close();
        return true;
    } else return false;
}

// delete neural network. Other methods won't work correctly
int NeuralNetwork::deletePermanently(const string &name) {
    const string PATH = "neural_networks\\" + name + '\\';
    int returnVal = -1;
    if (remove((PATH + "weights_and_biases.bin").c_str()) == 0 &&
        remove((PATH + "data.bin").c_str()) == 0) {
        returnVal = 0;
        if (rmdir(PATH.c_str()) != 0) return 1;
    }
    return returnVal;
}

// set a function comparing calculated & desired outputs and returning bool
void NeuralNetwork::setAnswersCheck(bool (*checkAnswers)(double *&calculated, double *&correct, int size)) {
    isCorrect = checkAnswers;
}

void NeuralNetwork::setTrainData(int count, double **&inputs, double **&outputs) {
    examplesCount = count;
    in = inputs;
    out = outputs;
}

void NeuralNetwork::setTestData(int count, double **&inputs, double **&outputs) {
    testExamplesCount = count;
    testIn = inputs;
    testOut = outputs;
    hasTestData = true;
}

const string &NeuralNetwork::getName() const {
    return NAME;
}

NeuralNetwork::ActivationFunction NeuralNetwork::getOutputLayerActivationFunctionType() const {
    return outputLayerActivationFunction->getType();
}


NeuralNetwork::BaseActivationFunction::BaseActivationFunction(NeuralNetwork &neuralNetwork,
                                                              ActivationFunction type) {
    NeuralNetwork::BaseActivationFunction::neuralNetwork = &neuralNetwork;
    BaseActivationFunction::type = type;
}

double NeuralNetwork::LogisticFunction::use(double x) {
    return 1 / (1 + exp(-x));
}

double NeuralNetwork::LogisticFunction::derivative(int layer, int neuron) {
    double value = neuralNetwork->a[layer + 1][neuron];
    return value * (1 - value);
}

double NeuralNetwork::ReLU::use(double x) {
    return max(0.0, x);
}

double NeuralNetwork::ReLU::derivative(int layer, int neuron) {
    return neuralNetwork->z[layer][neuron] >= 0;
}

double NeuralNetwork::LeakyReLU::use(double x) {
    return (x >= 0) ? x : PARAMETER * x;
}

double NeuralNetwork::LeakyReLU::derivative(int layer, int neuron) {
    return (neuralNetwork->z[layer][neuron] >= 0) ? 1 : PARAMETER;
}

double NeuralNetwork::LinearFunction::use(double x) {
    return x;
}

double NeuralNetwork::LinearFunction::derivative(int layer, int neuron) {
    return 1;
}

double NeuralNetwork::Tanh::use(double x) {
    return tanh(x);
}

double NeuralNetwork::Tanh::derivative(int layer, int neuron) {
    return 1 - pow(neuralNetwork->a[layer + 1][neuron], 2);
}

double NeuralNetwork::ArcTan::use(double x) {
    return atan(x);
}

double NeuralNetwork::ArcTan::derivative(int layer, int neuron) {
    return 1 / (1 + pow(neuralNetwork->z[layer][neuron], 2));
}

double NeuralNetwork::ReLU6::use(double x) {
    return max(0.0, min(6.0, x));
}

double NeuralNetwork::ReLU6::derivative(int layer, int neuron) {
    double value = neuralNetwork->z[layer][neuron];
    return value >= 0 && value <= 6;
}

double NeuralNetwork::ELU::use(double x) {
    return (x >= 0) ? x : (exp(x) - 1) * PARAMETER;
}

double NeuralNetwork::ELU::derivative(int layer, int neuron) {
    double value = neuralNetwork->a[layer + 1][neuron];
    return (value >= 0) ? 1 : value + PARAMETER;
}
