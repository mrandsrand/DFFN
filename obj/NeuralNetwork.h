class NeuralNetwork {
public:
    enum class ActivationFunction {
        LinearFunction = 0, ReLU, LeakyReLU, ELU, ReLU6, ArcTan, Tanh, LogisticFunction
    };

private:
    class BaseActivationFunction;

    double LEARN_RATE;
    std::string NAME;
    std::string path;

    int layersCount;
    int *layerSize;

    double **a; // values in neurons
    double **z; // values before putting them inside activation functions
    double **b; // biases
    double ***w; // weights

    double **db; // values used to twist biases to decrease cost
    double ***dw; // values used to twist weights to decrease cost

    BaseActivationFunction *outputLayerActivationFunction;
    BaseActivationFunction *hiddenLayerActivationFunction;

    double **in;
    double **out;
    int examplesCount;

    bool hasTestData;
    double **testIn;
    double **testOut;
    int testExamplesCount;

    bool (*isCorrect)(double *&calculated, double *&correct, int size);


    void setData();

    void setActivationFunction(BaseActivationFunction *&activationFunction, ActivationFunction setTo);

    void simpleTest(const std::string &testName, int &n, double **&x, double **&y);

    double cost(double *&expected);

    void backpropagation(double *&outputs);

    bool save();

public:
    explicit NeuralNetwork(const std::string &name, double learnRate = -1) noexcept(false);

    NeuralNetwork(const std::string &name, int examplesCount, double **&in, double **&out, int layersCount,
                  int *layerSize,
                  ActivationFunction outputLayerActivationFunction, ActivationFunction hiddenLayerActivationFunction,
                  bool (*checkAnswers)(double *&calculated, double *&correct, int size) = nullptr,
                  double learnRate = 1.0);

    ~NeuralNetwork();

    void learn(int learningSessionsPerInfo, int lessonsPerLearningSession = 100);

    double *calculate(double *&inputs);

    void test();

    bool reload(bool openAndRead = true);

    void setAnswersCheck(bool (*checkAnswers)(double *&calculated, double *&correct, int size));

    void setTrainData(int count, double **&inputs, double **&outputs);

    void setTestData(int count, double **&inputs, double **&outputs);

    const std::string &getName() const;

    ActivationFunction getOutputLayerActivationFunctionType() const;

    static int deletePermanently(const std::string &name);

// Activation functions nested classes
private:
    // all activation functions must inherit from it
    class BaseActivationFunction {
    protected:
        NeuralNetwork *neuralNetwork;
        ActivationFunction type;

        BaseActivationFunction(NeuralNetwork &neuralNetwork,
                                        ActivationFunction type);

    public:
        virtual ~BaseActivationFunction() = default;

        virtual double use(double x) = 0;

        virtual double derivative(int layer, int neuron) = 0;

        ActivationFunction getType() const { return type; };
    };

    class LogisticFunction : public BaseActivationFunction {
    public:
        explicit LogisticFunction(NeuralNetwork &neuralNetwork)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::LogisticFunction) {}

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class ReLU : public BaseActivationFunction {
    public:
        explicit ReLU(NeuralNetwork &neuralNetwork)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::ReLU) {}

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class LeakyReLU : public BaseActivationFunction {
    private:
        double PARAMETER;
    public:
        explicit LeakyReLU(NeuralNetwork &neuralNetwork, double parameter = 0.01)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::LeakyReLU) { PARAMETER = parameter; }

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class LinearFunction : public BaseActivationFunction {
    public:
        explicit LinearFunction(NeuralNetwork &neuralNetwork)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::LinearFunction) {}

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class Tanh : public BaseActivationFunction {
    public:
        explicit Tanh(NeuralNetwork &neuralNetwork)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::Tanh) {}

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class ArcTan : public BaseActivationFunction {
    public:
        explicit ArcTan(NeuralNetwork &neuralNetwork)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::ArcTan) {}

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class ReLU6 : public BaseActivationFunction {
    public:
        explicit ReLU6(NeuralNetwork &neuralNetwork)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::ReLU6) {}

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };

    class ELU : public BaseActivationFunction {
    private:
        double PARAMETER;
    public:
        explicit ELU(NeuralNetwork &neuralNetwork, double parameter = 1)
                : BaseActivationFunction(neuralNetwork, ActivationFunction::ELU) { PARAMETER = parameter; }

        double use(double x) override;

        double derivative(int layer, int neuron) override;
    };
};