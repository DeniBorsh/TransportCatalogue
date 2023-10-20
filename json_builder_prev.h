#pragma once
/*
#include "json.h"

#include <optional>

namespace json {
	class DictContext;
	class KeyContext;
	class ArrayContext;

	class Builder {
	public:
		Builder();
		Builder& Value(json::Node::Value value);
		DictContext StartDict();
		ArrayContext StartArray();
		KeyContext Key(std::string key);
		Builder& EndDict();
		Builder& EndArray();
		json::Node Build() const;

	private:
		Node root_{nullptr};
		std::vector<Node*> nodes_stack_;
		std::optional<std::string> key_{std::nullopt};
		Node GetNode(Node::Value value) const;
	};

	class KeyContext {
	public:
		KeyContext(Builder& builder) : builder_(builder) {}

		DictContext Value(json::Node::Value value);
		DictContext StartDict();
		ArrayContext StartArray();

	private:
		Builder& builder_;
	};

	class DictContext {
	public:
		DictContext(Builder& builder) : builder_(builder) {}

		KeyContext Key(std::string key);
		Builder& EndDict();

	private:
		Builder& builder_;
	};

	class ArrayContext {
	public:
		ArrayContext(Builder& builder) : builder_(builder) {}

		Builder& EndArray();
		ArrayContext Value(json::Node::Value value);
		DictContext StartDict();
		ArrayContext StartArray();

	private:
		Builder& builder_;
	};
}
*/

