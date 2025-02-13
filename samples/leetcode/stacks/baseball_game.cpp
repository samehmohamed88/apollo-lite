#include <string>
#include <iostream>
#include <vector>
#include <numeric>
#include <stack>

using namespace std;

class Solution {
public:
    int calPoints(vector<string>& ops)
    {
        stack<int> stack;
        int sum = 0;

        for (int i = 0; i < ops.size(); i++) {
            if (ops[i] == "+") {
                int first = stack.top();
                stack.pop();

                int second = stack.top();

                stack.push(first);

                stack.push(first + second);

                sum += first + second;
            }

            else if (ops[i] == "D") {
                sum += 2 * stack.top();
                stack.push(2 * stack.top());
            }

            else if (ops[i] == "C") {
                sum -= stack.top();
                stack.pop();
            }

            else {
                sum += stoi(ops[i]);
                stack.push(stoi(ops[i]));
            }
        }

        return sum;
    }
};

int main() {
    auto sol = Solution();

    auto operations = std::vector<std::string>{"5","2","C","D","+"};
    std::cout << sol.calPoints(operations);
    return 0;
}
