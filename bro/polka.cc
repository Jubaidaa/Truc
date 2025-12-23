#include "polka.h"
#include <iostream>
#include <cstdlib>
#include <string>

int cast_to_int(Expression* e) {
    if (Constant* c = dynamic_cast<Constant*>(e)) {
        return c->get_value();
    }
    std::cerr << "Error: " << e->to_string() << " is not an integer." << std::endl;
    exit(1);
}

Pair* cast_to_pair(Expression* e) {
    if (Pair* p = dynamic_cast<Pair*>(e)) {
        return p;
    }
    std::cerr << "Error: " << e->to_string() << " is not a pair." << std::endl;
    exit(1);
}

void Polka::push(int x) {
    stack.push_back(new Constant(x));
}

int Polka::get_value() {
    if (stack.empty()) {
        std::cerr << "Error: Stack is empty." << std::endl;
        exit(1);
    }
    return cast_to_int(stack.back());
}

void Polka::push(Expression* a, Expression* b) {
    stack.push_back(new Pair(a, b));
}

void Polka::push(std::string op) {
    if (op == "+") {
        if (stack.size() < 2) exit(1);
        int v2 = cast_to_int(stack.back()); stack.pop_back();
        int v1 = cast_to_int(stack.back()); stack.pop_back();
        push(v1 + v2);
    }
    else if (op == "-") {
        if (stack.size() < 2) exit(1);
        int v2 = cast_to_int(stack.back()); stack.pop_back();
        int v1 = cast_to_int(stack.back()); stack.pop_back();
        push(v1 - v2);
    }
    else if (op == "*") {
        if (stack.size() < 2) exit(1);
        int v2 = cast_to_int(stack.back()); stack.pop_back();
        int v1 = cast_to_int(stack.back()); stack.pop_back();
        push(v1 * v2);
    }
    else if (op == "/") {
        if (stack.size() < 2) exit(1);
        int v2 = cast_to_int(stack.back()); stack.pop_back();
        if (v2 == 0) exit(1);
        int v1 = cast_to_int(stack.back()); stack.pop_back();
        push(v1 / v2);
    }
    else if (op == "><") {
        if (stack.size() < 2) exit(1);
        Expression* e1 = stack.back(); stack.pop_back();
        Expression* e2 = stack.back(); stack.pop_back();
        stack.push_back(e1);
        stack.push_back(e2);
    }
    else if (op == "$1") {
        if (stack.empty()) exit(1);
        Pair* p = cast_to_pair(stack.back());
        stack.pop_back();
        stack.push_back(p->fst->clone());
        delete p;
    }
    else if (op == "$2") {
        if (stack.empty()) exit(1);
        Pair* p = cast_to_pair(stack.back());
        stack.pop_back();
        stack.push_back(p->snd->clone());
        delete p;
    }
    else if (op == ".") {
        if (stack.size() < 2) exit(1);
        Expression* e2 = stack.back(); stack.pop_back();
        Expression* e1 = stack.back(); stack.pop_back();
        push(e1, e2);
    }
    else if (op == ":") {
        if (stack.empty()) exit(1);
        stack.push_back(stack.back()->clone());
    }
    else {
        std::cerr << "Error: Unknown operator " << op << std::endl;
        exit(1);
    }
}

std::string Polka::to_string() const {
    std::string res = "[";
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        if (it != stack.rbegin()) {
            res += ",";
        }
        res += (*it)->to_string();
    }
    res += "]";
    return res;
}