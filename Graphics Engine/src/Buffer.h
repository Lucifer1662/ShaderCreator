#pragma once
#include <GL\glew.h>


template<unsigned int BUFFER_TYPE>
class Buffer {
	unsigned int bo;
	unsigned int size;
	unsigned int usage;
	Buffer(const Buffer& buffer) = delete;
public:

	void SetUsage(unsigned int usage) {
		Buffer::usage = usage;
	}

	Buffer(unsigned int usage = GL_DYNAMIC_DRAW) {
		SetUsage(usage);
		glGenBuffers(1, &bo);
	}

	Buffer(void* data, unsigned int sizeInBytes, unsigned int usage = GL_DYNAMIC_DRAW) : Buffer(usage) {
		SetData(data, sizeInBytes);
	}
	inline void Bind() {
		glBindBuffer(BUFFER_TYPE, bo);
	}
	void SetData(void* data, unsigned int sizeInBytes) {
		Bind();
		size = sizeInBytes;
		glBufferData(BUFFER_TYPE, sizeInBytes, data, usage);
	}
	void SetSubData(void* data, unsigned int sizeInBytes, unsigned int offset) {
		if (offset + sizeInBytes > size)
			ResizeBuffers(sizeInBytes, offset);
		Bind();		
		glBufferSubData(BUFFER_TYPE, offset, sizeInBytes, data);
		
	}

	Buffer& operator=(const Buffer& buffer) = delete;

	void BindBufferBase(int index) {
		glBindBufferBase(BUFFER_TYPE, index, bo);
	}
	

	void ResizeBuffers(unsigned int newSize, unsigned int copyAmount = size) {
		unsigned int vbotemp;
		Bind();

		//create a temp buffer of type GL_COPY_READ_BUFFER
		glGenBuffers(1, &vbotemp);
		glBindBuffer(GL_COPY_READ_BUFFER, vbotemp);
		glBufferData(GL_COPY_READ_BUFFER, newSize, nullptr, GL_STATIC_COPY);

		//copy data from vbo1 to it
		glCopyBufferSubData(BUFFER_TYPE, GL_COPY_READ_BUFFER, 0, 0, copyAmount);
		glBindBuffer(BUFFER_TYPE, vbotemp);
		//glVertexPointer(3, GL_FLOAT, 0, (char *)NULL);

		glDeleteBuffers(1, &bo);
		bo = vbotemp;
		size = newSize;

	}

	~Buffer(){
		glDeleteBuffers(1, &bo);
	}


};
