#pragma once
#include <vector>
#include <Program.h>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <array>
#include <functional>
#include "Characteristic.h"

struct Type {
	enum TypeEnum {
		Float,
		Int,
		Vec2,
		Vec3,
		Vec4,
		Mat4,
		Mat3,
		Size
	}type;
	static std::array<std::string, Type::TypeEnum::Size> names;
	static std::array<int, Type::TypeEnum::Size> layoutSize;
	static std::array<int, Type::TypeEnum::Size> numFloats;

	std::string getName() { return names[type]; }
	int getSizeBucket() { return layoutSize[type];  }
	int getNumFloats() { return numFloats[type]; }

	Type(TypeEnum type) :type(type) {}
	Type(const std::string& typeName) {
		auto it = std::find(names.begin(), names.end(), typeName);
		type = (TypeEnum)(it - names.begin());
	}
	Type(const char* typeName) :Type(std::string(typeName)) {
		
	}
};

struct Variable {
	Type type;
	std::string name;
	Variable(Type type, std::string name) :type(type), name(name) {}
	bool operator==(const Variable& l) { return l.name == name; }
};

std::array<std::string, Type::TypeEnum::Size> Type::names({ 
		"float",
		"int",
		"vec2",
		"vec3",
		"vec4",
		"mat4",
		"mat3"
});

std::array<int, Type::TypeEnum::Size> layoutSize = {
		1,
		1,
		1,
		1,
		1,
		4,
		3
};

std::array<int, Type::TypeEnum::Size> numFloats = {
		1,
		1,
		2,
		3,
		4,
		16,
		12
};



struct Layout : public Variable{
	int position;
	Layout(Variable var, int position) : Variable(var), position(position) {}
	std::string getDeclaration() { return "layout(location = "+std::to_string(position)+") in " + type.getName() + " " + name + ";"; }
};

struct Uniform : public Variable {
	Uniform(Variable var) : Variable(var) {}
	std::string getDeclaration() { return "uniform " + type.getName() + " " + name + ";"; }
};

/*
struct Uniform {
	Variable var;
	Uniform(Variable var):var(var){}
	std::string getDeclaration() { return "uniform " + var.type.getName() + " " + var.name + ";"; }
	bool operator==(const Uniform& u) { return var.name == u.var.name; }
};
*/

struct Shader {
	std::vector<Layout> layouts;
	int layoutSize = 0;
	std::vector<Uniform> uniforms;
	std::vector<std::string> global;

	std::vector<std::string> main;
	int uniquieNameCount = 0;


	void addToMain(const std::string& str) { main.emplace_back(str); }
	void addToGlobal(const std::string& str) { global.emplace_back(str); }
	void addLayout(const Layout& layout) {
		auto it = std::find(layouts.begin(), layouts.end(), layout);
		if (layouts.end() == it) {
			layouts.emplace_back(layout);
		}
	}
	void addUniform(const Uniform& uniform) {
		auto it = std::find(uniforms.begin(), uniforms.end(), uniform);
		if (uniforms.end() == it) {
			uniforms.emplace_back(uniform);
		}
	}
	std::string Construct() {
		std::string program;
		for (auto& layout : layouts) {
			program.append(layout.getDeclaration() + "\n");
		}
		for (auto& uniform : uniforms) {
			program.append(uniform.getDeclaration() + "\n");
		}
		std::for_each(global.begin(), global.end(), [&](auto& line) { program.append(line + "\n"); });

		program.append(std::string("main(){") + "\n");
		std::for_each(main.begin(), main.end(), [&](auto& line) { program.append("\t" +line + "\n");});
		program.append(std::string("}") + "\n");
		return program;
	}
	std::string getUniquieName() {
		uniquieNameCount++;
		return std::string("var") + std::to_string(uniquieNameCount);
	}

};




struct INode {	
	virtual Variable GetOutPut(Shader& shader, int i = 0) = 0;

	virtual void Reset() {}
};

struct Input {
	Input(INode* node = nullptr, int index = 0) : node(node), index(index) {}

	int index = 0;
	INode* node = nullptr;
	Variable Evaluate(Shader& shader) const { return node ? node->GetOutPut(shader, index) : Variable(Type::TypeEnum::Size, ""); }
	bool isGood() const { return node != nullptr; }
	void Reset() { if (isGood()) node->Reset(); }
};

template<int NumInputs, int NumOutsputs>
struct Node : public INode{
protected:
	std::array<Input, NumInputs> inputs;
	bool visited = false;

public:
	void SetInput(Input input, int i = 0) { inputs[i] = input; }
	void Reset() {
		visited = false;
		for (auto& input : inputs) {
			input.Reset();
		}
	}
	bool isGood() const {
		for (auto& input : inputs) {
			input.isGood();
		}
	}

};






struct LayoutIn : public Node<0,1> {
	Variable var;
	LayoutIn(Variable var): var(var) {}
	Variable GetOutPut(Shader& shader, int i) {
		if (!visited) {
			shader.addLayout(Layout(var, shader.layoutSize));
			shader.layoutSize += 1;
			visited = true;
		}
		return var;
	}
};

struct UniformIn : public Node<0,1> {
	bool isCustom = false;
	Variable var;
	UniformIn(Variable var) :var(var), isCustom(var.name != "") {}
	Variable GetOutPut(Shader& shader, int i) {
		if (!visited) {
			if(var.name == "") var.name = shader.getUniquieName();
			shader.addUniform(Uniform(var));
			visited = true;
		}
		return var;
	}
	
	void Reset() {
		if(!isCustom)
			var.name = "";
	}
};

struct PassIn : public Node<0, 1> {
	bool isCustom = false;
	Variable var;
	PassIn(Variable var) :var(var), isCustom(var.name != "") {}
	Variable GetOutPut(Shader& shader, int i) {
		if (!visited) {
			if(var.name == "") var.name = shader.getUniquieName();
			shader.addToGlobal("in " + var.type.getName() + " " + var.name + ";");
			visited = true;
		}
		return var;
	}

	void Reset() {
		if (!isCustom)
			var.name = "";
	}
};

struct PassOut : public Node<1, 0> {
	bool isCustom = false;
	Variable var;
	PassOut(Variable var) :var(var), isCustom(var.name != "") {}
	Variable GetOutPut(Shader& shader, int i) {
		if (!visited) {
			if (var.name == "") var.name = shader.getUniquieName();
			shader.addToGlobal("out " + var.type.getName() + " " + var.name + ";");
			shader.addToMain(var.type.getName() + " " + var.name + " = " + inputs[0].Evaluate(shader).name);
			visited = true;
		}
		return var;
	}

	void Reset() {
		if (!isCustom)
			var.name = "";
	}
};

struct PositionOut : public Node<1,0> {
	Variable GetOutPut(Shader& shader, int i) {
		if (!visited) {
			auto right = inputs[0].Evaluate(shader);
			shader.addToMain("gl_position=" + right.name + ";");
			visited = true;
		}
		return Variable(Type::TypeEnum::Vec4, "gl_position");
	}
};

struct Multiplication : public Node<2,1> {
	Variable var = Variable(Type::TypeEnum::Size, "");
	Variable GetOutPut(Shader& shader, int i) {
		if (!visited) {
			var.name = shader.getUniquieName();
			auto left = inputs[0].Evaluate(shader);
			auto right = inputs[1].Evaluate(shader);
			shader.addToMain(right.type.getName() + " " + var.name + "=" + left.name + "*" + right.name + ";");
			visited = true;
		}
		return var;
	}

	void Reset() {
		var.name = "";
	}
};

template <int NumInputs>
struct FuncNode : public Node<NumInputs, 1> {
	Variable var;
	string function;
	FuncNode(Variable var, string function) : var(var), function(function) {}

	Variable GetOutPut(Shader& shader, int i) {
		if (!Node<NumInputs, 1>::visited) {
			var.name = function + "(";
			for (size_t i = 0; i < Node<NumInputs,1>::inputs.size(); i++){
				var.name += Node<NumInputs, 1>::inputs[i].Evaluate(shader).name;
				if (i + 1 != Node<NumInputs, 1>::inputs.size())
					var.name += ",";
			}
			var.name += ")";
			Node<NumInputs, 1>::visited = true;
		}
		return var;
	}

	void Reset() {
		var.name = "";
	}
};

struct ConstantVariable : public Node<0,1> {
	Variable var;
	ConstantVariable(Variable var) : var(var) {}
	Variable GetOutPut(Shader& shader, int i) {
		return var;
	}
};


//enum NodeEnum{
//	PositionIn,
//	PositionOut
//}
//
//INode make_node(NodeEnum node) {
//	switch (node){
//	case NodeEnum::PositionIn:
//		return PositionIn();
//		break;
//	}
//}



auto LayoutStart(int position, int numFloats, int stride, int offset, int type = GL_FLOAT, int normalize = GL_FALSE) {
	return [=]() {
		glEnableVertexAttribArray(position);
		glVertexAttribPointer(position, numFloats, GL_FLOAT, normalize, stride, (void*)offset);
	};
}

auto LayoutEnd(int position) {
	return [=]() {
		glDisableVertexAttribArray(position);
	};
}



struct Material
{
	std::vector<std::function<void()>> activates;
	std::vector<std::function<void()>> deacativates;

	void Render() {

		for (auto& activate : activates) {
			activate();
		}

		for (auto& deacativate : deacativates) {
			deacativate();
		}
	}
};



void test3() {
	auto posIn = LayoutIn({ "vec3", "position" });
	auto uvIn = LayoutIn({ "vec2", "uv" });
	auto uvOut = PassOut({ "vec2", "_uv" });
	auto transformUniform = UniformIn({ "mat4","transformMat" });
	auto posOut = PositionOut();
	auto mulplication = Multiplication();
	auto number1 = ConstantVariable({ "int","1" });
	auto positionToVec4 = FuncNode<2>({ "vec4", "" }, "vec4");

	uvOut.SetInput(&uvIn);
	positionToVec4.SetInput(&posIn, 0);
	positionToVec4.SetInput(&number1, 1);


	mulplication.SetInput(&transformUniform, 0);
	mulplication.SetInput(&positionToVec4, 1);
	posOut.SetInput(&mulplication);
	Shader vertexShader;
	posOut.GetOutPut(vertexShader, 0);
	uvOut.GetOutPut(vertexShader, 0);
	std::cout << vertexShader.Construct();

	Material mat;
	int stride = 0;
	for (auto& layout : vertexShader.layouts) {
		layout.type.getNumFloats();
	}
	int offset = 0;
	for (auto& layout : vertexShader.layouts) {
		mat.activates.push_back(LayoutStart(layout.position, layout.type.getNumFloats(), stride, offset));
		offset += layout.type.getNumFloats();
		mat.deacativates.push_back(LayoutEnd(layout.position));
	}
	

	mat.Render();
}

