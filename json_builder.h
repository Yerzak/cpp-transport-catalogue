#pragma once
#include "json.h"
#include <vector>
#include <string>
#include <variant>
#include <map>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <stack>
namespace json {
    class BaseContext;
    class DictItemContext;
    class ArrItemContext;
    class KeyItemContext;
    class Builder {
    private:
        Node root_;
        std::stack<Node*> nodes_stack_;
        std::string key_ = "Not a key";
    public:
        Builder();
        DictItemContext StartDict();
        ArrItemContext StartArray();
        KeyItemContext Key(std::string word);
        BaseContext EndDict();
        BaseContext EndArray();
        BaseContext Value(Node::Value val);
        Node Build();
    };//class Builder

    class BaseContext {
    public:
        BaseContext(Builder& builder);
        DictItemContext StartDict();
        ArrItemContext StartArray();
        KeyItemContext Key(std::string word);
        BaseContext EndDict();
        BaseContext EndArray();
        BaseContext Value(Node::Value val);
        Node Build();
    private:
        Builder& b;
    };

    class DictItemContext : public BaseContext {
    public:
        DictItemContext(BaseContext base);
        DictItemContext StartDict() = delete;
        ArrItemContext StartArray() = delete;
        BaseContext EndArray() = delete;
        BaseContext Value(Node::Value val) = delete;
        Node Build() = delete;
    };//class DictItemContext

    class ArrItemContext : public BaseContext {
    public:
        ArrItemContext(BaseContext base);
        ArrItemContext Value(Node::Value val);////////////////////////////////////

        KeyItemContext Key(std::string word) = delete;
        BaseContext EndDict() = delete;
        Node Build() = delete;
    };//class ArrItemContext

    class KeyItemContext : public BaseContext {
    public:
        KeyItemContext(BaseContext base);
        DictItemContext Value(Node::Value val);/////////////////////////////////

        BaseContext EndArray() = delete;
        KeyItemContext Key(std::string word) = delete;
        BaseContext EndDict() = delete;
        Node Build() = delete;
    };//class KeyItemContext

} //namespace json