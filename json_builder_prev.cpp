/*
#include "json_builder_prev.h"
#include <iostream>

using namespace std;

namespace json {

	Builder::Builder() {
		nodes_stack_.emplace_back(&root_);
	}

	KeyContext json::Builder::Key(string key) {
		if (nodes_stack_.back()->IsDict() && !key_) key_ = std::move(key);
		else throw std::logic_error("Wrong map key: " + key);

		return *this;
	}

	Builder& Builder::Value(Node::Value value) {
		Node* top_node = nodes_stack_.back();

		if (top_node->IsDict()) {
			if (!key_) throw logic_error("Ключ не задан");
			Dict& dict = get<Dict>(top_node->GetValue());
			auto [pos, _] = dict.emplace(move(key_.value()), GetNode(move(value)));
			key_ = nullopt;
		}
		else if (top_node->IsArray()) {
			Array& array = get<Array>(top_node->GetValue());
			array.emplace_back(GetNode(move(value)));
		}
		else if (root_.IsNull()) {
			root_.GetValue() = move(move(value));
		}
		else throw logic_error("Value() вызывается в неизвестном контейнере");

		return *this;
	}

	
	DictContext Builder::StartDict() {
		Node* top_node = nodes_stack_.back();

		if (top_node->IsDict()) {
			if (!key_) throw logic_error("Ключ не задан");
			Dict& dict = get<Dict>(top_node->GetValue());
			auto [pos, _] = dict.emplace(move(key_.value()), Dict());
			key_ = nullopt;
			nodes_stack_.emplace_back(&pos->second);
		}
		else if (top_node->IsArray()) {
			Array& array = get<Array>(top_node->GetValue());
			array.emplace_back(Dict());
			nodes_stack_.emplace_back(&array.back());
		}
		else if (root_.IsNull()) {
			top_node->GetValue() = Dict();
		}
		else throw logic_error("Ошибка при объявлении словаря");

		return *this;
	}

	ArrayContext Builder::StartArray() {
		Node* top_node = nodes_stack_.back();

		if (top_node->IsDict()) {
			if (!key_) throw logic_error("Ключ не задан");
			Dict& dict = get<Dict>(top_node->GetValue());
			auto [pos, _] = dict.emplace(move(key_.value()), Array());
			key_ = nullopt;
			nodes_stack_.emplace_back(&pos->second);
		}
		else if (top_node->IsArray()) {
			Array& array = get<Array>(top_node->GetValue());
			array.emplace_back(Array());
			nodes_stack_.emplace_back(&array.back());
		}
		else if (root_.IsNull()) {
			top_node->GetValue() = Array();
		}
		else throw logic_error("Ошибка при объявлении словаря");

		return *this;
	}

	Builder& Builder::EndDict() {
		Node* top_node = nodes_stack_.back();

		if (!top_node->IsDict()) throw logic_error("Словарь не открыт");

		nodes_stack_.pop_back();

		return *this;
	}

	Builder& Builder::EndArray() {
		auto* top_node = nodes_stack_.back();

		if (!top_node->IsArray()) throw logic_error("Prev node is not an Array");

		nodes_stack_.pop_back();

		return *this;
	}

	Node Builder::Build() const {
		if (root_.IsNull() || nodes_stack_.size() > 1) throw logic_error("Не все операции завершены");

		return root_;
	}

	Node Builder::GetNode(Node::Value value) const {
		if (holds_alternative<int>(value)) return Node(get<int>(value));
		if (holds_alternative<double>(value)) return Node(get<double>(value));
		if (holds_alternative<string>(value)) return Node(get<string>(value));
		if (holds_alternative<nullptr_t>(value)) return Node(get<nullptr_t>(value));
		if (holds_alternative<bool>(value)) return Node(get<bool>(value));
		if (holds_alternative<Dict>(value)) return Node(get<Dict>(value));
		if (holds_alternative<Array>(value)) return Node(get<Array>(value));
		return {};
	}

	KeyContext DictContext::Key(string key) {
		return builder_.Key(key);
	}

	Builder& DictContext::EndDict() {
		return builder_.EndDict();
	}

	DictContext KeyContext::Value(json::Node::Value value) {
		DictContext tmp = { builder_.Value(value) };
		return tmp;
	}

	DictContext KeyContext::StartDict() {
		return builder_.StartDict();
	}

	ArrayContext KeyContext::StartArray() {
		return builder_.StartArray();
	}

	Builder& ArrayContext::EndArray() {
		return EndArray();
	}

	ArrayContext ArrayContext::Value(json::Node::Value value) {
		ArrayContext tmp = { builder_.Value(value) };
		return tmp;
	}

	DictContext ArrayContext::StartDict() {
		return builder_.StartDict();
	}
	ArrayContext ArrayContext::StartArray() {
		return builder_.StartArray();
	}
}
*/

