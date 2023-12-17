#include "json_builder.h"
namespace json {

    /*-------------------------------BUILDER---------------------------------------*/
    Builder::Builder() {//конструктор
        nodes_stack_.push(&root_);//root - через элементы в стеке мы по сути заполняем наш Node root_;
    }
    DictItemContext Builder::StartDict() {
        if (root_.IsNull()) {
            nodes_stack_.top()->GetRawValue() = Dict{};
        }
        else {
            if (nodes_stack_.top()->IsDict()) {
                if (key_ != "Not a key") {
                    const auto& it_bool = std::get<Dict>(nodes_stack_.top()->GetRawValue()).emplace(key_, Node(Dict{}));
                    auto& ref = *it_bool.first;
                    nodes_stack_.push(&ref.second);
                    key_ = "Not a key";
                }
                else {
                    throw std::logic_error("there is no any key to start new map in current map");
                }
            }
            else if (nodes_stack_.top()->IsArray()) {
                auto& iter = std::get<Array>(nodes_stack_.top()->GetRawValue()).emplace_back(Node(Dict{}));
                nodes_stack_.push(&iter);
            }
            else {
                throw std::logic_error("you cannot start map");
            }
        }
        return DictItemContext(*this);
    }
    ArrItemContext Builder::StartArray() {
        if (root_.IsNull()) {
            nodes_stack_.top()->GetRawValue() = Array();
        }
        else {
            if (nodes_stack_.top()->IsDict()) {
                if (key_ != "Not a key") {
                    const auto& it_bool = std::get<Dict>(nodes_stack_.top()->GetRawValue()).emplace(key_, Node(Array{}));
                    auto& ref = *it_bool.first;
                    nodes_stack_.push(&ref.second);
                    key_ = "Not a key";
                }
                else {
                    throw std::logic_error("there is no key to start new array in current map");
                }
            }
            else if (nodes_stack_.top()->IsArray()) {
                auto& iter = std::get<Array>(nodes_stack_.top()->GetRawValue()).emplace_back(Node(Array{}));
                nodes_stack_.push(&iter);
            }
            else {
                throw std::logic_error("You cannot start new array");
            }
        }
        return ArrItemContext{ *this };
    }
    KeyItemContext Builder::Key(std::string word) {
        if (key_ == "Not a key") {
            key_ = std::move(word);
        }
        else {
            throw std::logic_error("Key is exist yet");
        }
        return KeyItemContext{ *this };
    }
    BaseContext Builder::EndDict() {
        if (nodes_stack_.top()->IsDict()) {
            nodes_stack_.pop();
        }
        else {
            throw std::logic_error("You cannot end the dictionary");
        }
        return BaseContext(*this);
    }
    BaseContext Builder::EndArray() {
        if (nodes_stack_.top()->IsArray()) {
            nodes_stack_.pop();
        }
        else {
            throw std::logic_error("you cannot end the array");
        }
        return BaseContext(*this);
    }
    BaseContext Builder::Value(Node::Value val) {
        if (nodes_stack_.top()->IsDict()) {
            if (key_ != "Not a key") {
                std::get<Dict>(nodes_stack_.top()->GetRawValue()).emplace(key_, val);
                key_ = "Not a key";
            }
            else {
                throw std::logic_error("you have not any key");
            }
        }
        else if (nodes_stack_.top()->IsArray()) {
            auto& host_value = nodes_stack_.top()->GetRawValue();
            std::get<Array>(host_value).emplace_back(std::move(val));
        }
        else if (root_.IsNull()) {
            nodes_stack_.top()->GetRawValue() = std::move(val);
        }
        else {
            throw std::logic_error("value is loaded yet or dict (arr) is not opened");
        }
        return BaseContext{ *this };
    }
    Node Builder::Build() {
        if (root_.IsNull() || (!root_.IsNull() && nodes_stack_.size() > 1)) {
            throw std::logic_error("Object is empty");
        }
        else if (key_ != "Not a key") {
            throw std::logic_error("key...");
        }
        return root_;
    }
    /*--------------------BASE CONTEXT-----------------------------*/

    BaseContext::BaseContext(Builder& builder)
        : b(builder) {
    }
    DictItemContext BaseContext::StartDict() {
        return b.StartDict();
    }
    ArrItemContext BaseContext::StartArray() {
        return b.StartArray();
    }
    KeyItemContext BaseContext::Key(std::string word) {
        return b.Key(word);
    }
    BaseContext BaseContext::EndDict() {
        return b.EndDict();
    }
    BaseContext BaseContext::EndArray() {
        return b.EndArray();
    }
    BaseContext BaseContext::Value(Node::Value val) {
        return b.Value(val);
    }
    Node BaseContext::Build() {
        return b.Build();
    }
    /*------------------------DICT ITEM CONTEXT-------------------*/
    DictItemContext::DictItemContext(BaseContext base)
        : BaseContext(base) {
    }

    /*--------------------ARR ITEM CONTEXT------------------------*/
    ArrItemContext::ArrItemContext(BaseContext base)
        : BaseContext(base) {
    }
    ArrItemContext ArrItemContext::Value(Node::Value val) {
        return ArrItemContext(BaseContext::Value(val));
    }
    /*-------------------KEY ITEM CONTEXT---------------------------*/
    KeyItemContext::KeyItemContext(BaseContext base)
        : BaseContext(base) {
    }
    ValueItemContext KeyItemContext::Value(Node::Value val) {
        return ValueItemContext(BaseContext::Value(val));
    }
    /*-------------------VALUE ITEM CONTEXT-----------------*/
    ValueItemContext::ValueItemContext(BaseContext base)
        : BaseContext(base) {
    }

}//namespace json