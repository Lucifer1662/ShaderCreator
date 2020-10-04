#pragma once
#include <vector>
#include <string>

struct LineOfCode {
	std::string name;
	std::string code;
	LineOfCode(std::string name, std::string code)
		:name(name), code(code) {}

	bool operator==(const LineOfCode& line) {
		return line.name == name;
	}
};

void merge(std::vector<LineOfCode>& left, const std::vector<LineOfCode>& right) {
	auto start = right.begin();
	auto end = start;
	while (start != right.end()) {
		for (; end != right.end(); end++) {
			auto res = std::find(left.begin(), left.end(), *end);
			if (res != left.end()) {
				//found start
				left.insert(res, start, end);
				end++;
				start = end;
				break;
			}
		}
	}
}


struct Characteristic {
	virtual std::vector<LineOfCode> GetVertexShader() { return {}; }
	virtual std::vector<LineOfCode> GetFragmentShader() { return {}; }
};

class PositionCharacteristic : public Characteristic {
	int locationIndex;
public:
	std::vector<LineOfCode> GetVertexShader() {
		return {
		{"version", "#version 330 core" },
		{"layoutPostion", "layout (location = " + std::to_string(locationIndex) + ") in vec3 pos;" },
		{"main start", "main(){" },
			{"gl_position set", "gl_Position = vec4(pos,1);" },
		{"main end", "}" }
	}; }
}; 


class CameraCharacteristic : public PositionCharacteristic {
public:
	std::vector<LineOfCode> GetVertexShader() {
		auto cameraCode = std::vector<LineOfCode>(
			{
			{"version", "#version 330 core" },
			{"camera uniform", "uniform mat4 camera;" },
			{"main start", "main(){" },
				{"gl_position set", "gl_Position = vec4(pos,1);" },
				{"camera apply", "gl_Position = camera * gl_Position;" },
			{"main end", "}" }
			});
		auto position = PositionCharacteristic::GetVertexShader();
		merge(position, cameraCode);
		return position;
	}
};



std::vector<LineOfCode> PositionVertexShader(int locationIndex) {
	return {
		{"version", "#version 330 core" },
		{"layoutPostion", "layout (location = " + std::to_string(locationIndex) + ") in vec3 pos;" },
		{"main start", "main(){" },
			{"gl_position set", "gl_Position = vec4(pos,1);" },
		{"main end", "}" }
	};
}

std::vector<LineOfCode> CameraPositionVertexShader(int locationIndex) {
	return {
		{"version", "#version 330 core" },
		{"layoutPostion", "layout (location = " + std::to_string(locationIndex) + ") in vec3 pos;" },
		{"camera uniform", "uniform mat4 camera;" },
		{"main start", "main(){" },
			{"gl_position set", "gl_Position = vec4(pos,1);" },
			{"camera apply", "gl_Position = camera * gl_Position;" },
		{"main end", "}" }
	};
}

std::vector<LineOfCode> UvVertexShader(int locationIndex) {
	return {
		{"version", "#version 330 core" },
		{"layoutUv", "layout (location = " + std::to_string(locationIndex) + ") in vec2 uv;" },
		{"uv out", "out vec2 _uv;" },
		{"main start", "main(){" },
			{"uv set", "  _uv = uv;" },
		{"main end", "}" }
	};
}

