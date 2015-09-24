question1: GLApplication.cpp AdjustableCamera.h Material.h Mesh.h StaticMesh.h Mesh.cpp StaticMesh.cpp shaders/question1_student_code.h
	g++ GLApplication.cpp AdjustableCamera.h Material.h Mesh.h StaticMesh.h Mesh.cpp StaticMesh.cpp shaders/question1_student_code.h -o question1 -I/usr/include -L/usr/lib -lGLEW -lglut -lGL -lGLU -lX11 -g
clean:
	-rm -f question1
