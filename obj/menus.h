Menu preMenu("START", vector<Option> {
    Option("              Press /ENTER/ to continue             ")
});

Menu menu("MENU", vector<Option>{
        Option("Get started"),
        Option("Learn more", "worth reading before getting started"),
        Option("Credits"),
        Option("Exit")
});

    Menu menu_0("Select neural network", vector<Option>{
            Option("New..."),
            Option("Back")
    }, 1);

        Menu menu_0_0("Select activation function on hidden layer(s).", vector<Option>{
                Option("Linear Function", "f(x) = x; values from -inf to inf"),
                Option("ReLU", "f(x >= 0) = x; f(x < 0) = 0; values from 0 to inf"),
                Option("Leaky ReLU", "f(x >= 0) = x; f(x < 0) = 0.01x; values from -inf to inf"),
                Option("ELU", "f(x >= 0) = x; f(x < 0) = e^x - 1; values from -inf to inf"),
                Option("ReLU-6", "f(0 <= x <= 6) = x; f(x < 0) = 0; f(x > 6) = 6"),
                Option("Arcus tangent", "f(x) = atan(x); values between -pi/2 & pi/2"),
                Option("Hyperbolic tangent", "f(x) = tanh(x); values between -1 & 1"),
                Option("Logistic function", "f(x) = 1 / (1 + e^(-x)); values between 0 & 1")
        });

            Menu menu_0_0_0("Select activation function on output layer.", vector<Option>{
                    Option("Logistic function", "f(x) = 1 / (1 + e^(-x)); values between 0 & 1"),
                    Option("Hyperbolic tangent", "f(x) = tanh(x); values between -1 & 1"),
                    Option("Arcus tangent", "f(x) = atan(x); values between -pi/2 & pi/2"),
                    Option("ReLU-6", "f(0 <= x <= 6) = x; f(x < 0) = 0; f(x > 6) = 6")
            });

                Menu menu_0_0_0_0("Are you happy with this neural network?", vector<Option>{
                        Option("Yes", "create"),
                        Option("No", "discard"),
                        Option("Back", "to main menu", true)
                }, 1);

        Menu menu_0_1("What would you like to do?", vector<Option>{
                Option("Start learning process"),
                Option("Run some tests"),
                Option("Delete"),
                Option("Change neural network"),
                Option("Back", "to main menu", true)
        }, 3);

            Menu menu_0_1_1("How do you want to test your neural network?", vector<Option> {
                Option("Draw some digits", "in MS Paint"),
                Option("On normalized data"),
                Option("Back")
            }, 2);

                Menu menu_0_1_1_0("\n"
                                  " When the window appears, do NOT resize the drawing sheet.\n"
                                  " Instead, zoom it up to 800% (in the right-bottom corner).\n\n"
                                  " Use default brush to draw the digit, and don't be afraid\n"
                                  " to make your creation big - just avoid touching the borders.\n"
                                  " Try to make it look as if it was truly handwritten.\n"
                                  " When it's finished, just exit Paint and be sure to save.\n", vector<Option> {
                    Option("Got it"),
                    Option("Give me some example"),
                    Option("Go back")
                }, 2);

                    Menu menu_0_1_1_0_0("PERCENTAGE", vector<Option> {
                        Option("Let's try again"),
                        Option("Leave")
                    }, 1);

                    Menu menu_0_1_1_0_1("What digit do you want to see?", vector<Option> {
                        Option("Random"),
                        Option("0"),
                        Option("1"),
                        Option("2"),
                        Option("3"),
                        Option("4"),
                        Option("5"),
                        Option("6"),
                        Option("7"),
                        Option("8"),
                        Option("9"),
                        Option("Back")
                    }, 11);

                        Menu menu_0_1_1_0_1_0("DIGIT", vector<Option> {
                            Option("Thanks")
                        }, 0);

            Menu menu_0_1_2("Are you sure you want to permanently delete this neural network?", vector<Option> {
                Option("No", "let's give it another chance"),
                Option("Yes", "delete all files")
            }, 0);

                Menu menu_0_1_2_1("SUCCESS?", vector<Option> {
                    Option("OK")
                }, 0);

    Menu menu_1("Read about:", vector<Option> {
            Option("Your goal here"),
            Option("MNIST database"),
            Option("Neural networks"),
            Option("DFF Networks"),
            Option("Learning process"),
            Option("Never mind", "just go back")
    }, 5);

    string nn_example();
    Menu info[]{
        Menu ("\n\n\n\n"
            " Your goal is to create your own neural network (NN),\n"
            " which should be able to recognize handwritten digits.\n"
            " Definitely not in every scenario - the digits themselves\n"
            " have to be normalized to make NN categorise them properly.\n"
            " To make that happen, program will use the MNIST database\n"
            " to feed NN with data.\n\n\n"
            " Your neural networks may or may not work great, or even not work at all,\n"
            " depending on specification you choose during creation (and a bit of luck).\n"
            " It is part of the fun to play with different settings,\n"
            " try numerous combinations and find some that work the best.\n"
            " If you want to learn more about some nuances, just google them.\n\n\n"
            " And don't forget to have fun!\n\n\n\n", vector<Option> {
                Option("Next    ", "MNIST database"),
                Option("Back")
        }, 1),

        Menu ("\n\n\n\n"
            " The MNIST database of handwritten digits\n"
            " contains the very thing it states inside its own name:\n"
            " digits, written by real people, gathered and normalized,\n"
            " so that they can be used to test classification algorithms.\n\n\n"
            " It consists of a training set (60 000 examples) used to feed these algorithms\n"
            " (e.g. neural networks) and a test set (10 000 examples) to check the accuracy\n"
            " of predictions. Both come with labels corresponding to correct digit.\n"
            " Digits are stored as 28x28 black&white images\n"
            " with gray levels due to applied antialiasing.\n"
            " What's also important, each digit is centered\n"
            " in a computed center of mass of the pixels.\n\n\n"
            " You even saw one of the MNIST digits on a title screen!\n\n\n\n", vector<Option> {
                Option("Next", "neural networks"),
                Option("Previous", "your goal here"),
                Option("Back", {}, true)
        }, 1),

        Menu ("\n\n\n\n"
            " Neural networks may be defined as biologically-inspired\n"
            " computing systems with ability to find hidden patterns\n"
            " inside given data, without any prior knowledge on its properties.\n"
            " In other words, NNs can learn as we do - as long as they know\n"
            " if their predictions are going in a right direction,\n"
            " they can slowly but surely increase percentage of correct answers.\n\n\n"
            " NNs, just like our brains, consist of neurons & connections between them,\n"
            " and in machine learning these connections are modeled as weights\n"
            " - multipliers that may increase or decrease the strength of activity\n"
            " in some neurons. Modified activities (also numbers) in grouped neurons\n"
            " are summed up with a bias (more variables), normalized by activation function\n"
            " and that's how next neuron's activity is calculated.\n"
            " Learning is a process of twisting weights and biases, so that the outputs\n"
            " of NN are as close to real answers as possible.\n\n\n\n", vector<Option> {
                Option("Next", "DFF Networks"),
                Option("Previous", "MNIST database"),
                Option("Back", {}, true)
        }, 1),

        Menu (nn_example() + "\n"
              " What you see here ia a simple example of Deep Feedforward (DFF) Network.\n"
              " These NNs consist of layers (neurons grouped as vectors), specifically:\n"
              " * an input layer (left) - data of a single example organised as a vector;\n"
              " * hidden layers (middle) - values in these neurons can be happily ignored;\n"
              " * an output layer (right) - results of computations are stored in here.\n"
              " A value in each neuron is a sum of products of previous neurons (on the left)\n"
              " and corresponding weights, summed up with a bias assigned to this very neuron,\n"
              " and all that is wrapped in some activation function, which returns an output\n"
              " in desired boundaries. For example, a value in neuron a11 is some\n"
              "               f(a00 * w001 + a01 * w011 + b01)\n"
              " Note that it gets more complex with not 2 neurons in input layer,\n"
              " but 784 - the exact number of pixels in digit images from MNIST.\n"
              " Each pixel is treated as one input - a value from 0 (black) to 1 (white).", vector<Option> {
                Option("Next", "learning process"),
                Option("Previous", "neural networks"),
                Option("Back", {}, true)
        }, 1),
        
        Menu ("\n\n"
              " A measure telling us exactly how distant are NN's\n"
              " predictions from the desired answer is called 'cost'.\n"
              " For each neuron in the output layer we may calculate\n"
              " an error defined as: 0.5 * (output - desired_output)^2\n"
              " Cost is a sum of these errors and it's NNs' job to make it\n"
              " as close to zero as possible.\n\n"
              " The value itself means nothing to NNs, but partial derivatives\n"
              " of cost function with respect to weights and biases are another story.\n"
              " An algorithm used to train NNs here is called 'backpropagation'.\n"
              " It calculates these derivatives starting from the output layer\n"
              " and propagates backwards until all are gathered in a gradient\n"
              " - a vector pointing to the direction where a slope of a function\n"
              " grows the most quickly. Of course we don't want to increase the cost,\n"
              " that's why we use negative gradient of a cost function so that\n"
              " the errors could get smaller and smaller. An algorithm that\n"
              " from some input vector subtracts the average gradient based on\n"
              " e.g. 100 examples is called 'the gradient descend'. To increase\n"
              " the speed of finding some fine minimum of the cost function\n"
              " a 'learn rate' variable may be twisted to scale the gradient.\n\n", vector<Option> {
                Option("Back", {}, true),
                Option("Previous", "DFF Networks")
        }, 1)
    };

    Menu menu_2("\n"
                " Many thanks to my dear friend Maciej,\n"
                " for his irreplaceable guidance in this journey\n"
                " to comprehend basics of C++.\n\n"
                " Also, I must mention the videos that taught me\n"
                " almost everything I know about machine learning\n"
                " and made me create this very project, even though\n"
                " the concept of neural networks seemed so abstract\n"
                " and complex back in a day.\n"
                " Please visit \"3blue1brown\" on YouTube and feel free\n"
                " to stay for longer ;)\n", vector<Option>{
            Option("Back")
    }, 0);

    Menu menu_3("Are you sure you want to exit?", vector<Option>{
            Option("No", "stay"),
            Option("I am", "exit the program")
    }, 0);

// just a nice example of DFF neural network's structure
string nn_example() {
    char lu = -38, ld = -64, ru = -65, rd = -39, ve = -77, ho = -60,
            rp = -61, lp = -76, up = '/', dn = '\\', sp = ' ', nl = '\n';
    string pls_work = string() +
                           sp + sp + sp + sp + sp + sp + sp + sp +       "+b00"      + nl +
                      sp + lu + ho + ho + ho + ru +       "w000"      + lu + ho + ho + ho + ru + nl +
                      sp + ve +     "a00"    + rp + ho + ho + ho + ho + lp +     "a10"    + ve + nl +
                      sp + ld + ho + ho + ho + rd + dn + sp + sp + up + ld + ho + ho + ho + rd + dn + sp +       "+b10"      + nl +
                      sp + sp + sp +       "w001"      + dn + up + sp + sp + sp +       "w100"      + dn + lu + ho + ho + ho + ru + nl +
                      sp + sp + sp +       "w010"      + up + dn + sp + sp + sp +       "w110"      + up + ve +     "a20"    + ve + nl +
                      sp + lu + ho + ho + ho + ru + up + sp + sp + dn + lu + ho + ho + ho + ru + up + sp + ld + ho + ho + ho + rd + nl +
                      sp + ve +     "a01"    + rp + ho + ho + ho + ho + lp +     "a11"    + ve + nl +
                      sp + ld + ho + ho + ho + rd +       "w011"      + ld + ho + ho + ho + rd + nl +
                      sp + sp + sp + sp + sp + sp + sp + sp + sp +       "+b01"      + nl;
    return pls_work;
}