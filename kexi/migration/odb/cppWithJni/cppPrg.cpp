#include <stdio.h>
#include <jni.h>


JNIEnv* create_vm(JavaVM ** jvm) {
	
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options;
    options.optionString = "-Djava.class.path=/home/dharu/cppWithJni/cppWithJni2"; //Path to the java source code
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;
    
    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    if(ret < 0)
    	printf("\nUnable to Launch JVM\n");   	
	return env;
}

int main(int argc, char* argv[])
{
	JNIEnv *env;
	JavaVM * jvm;
	env = create_vm(&jvm);
	if (env == NULL)
		return 1;	

	
    jclass clsH=NULL;
    jmethodID midMain = NULL;
    
  
    //Obtaining Classes
    clsH = env->FindClass("helloWorld");
    
	//Obtaining Method IDs
    if (clsH != NULL)
    {
		midMain       = env->GetStaticMethodID(clsH, "main", "([Ljava/lang/String;)V");
	}
	else
    {
    	printf("\nUnable to find the requested class\n");    	
    }
	

	
	//call function using their method id
	if(midMain != NULL)
		env->CallStaticVoidMethod(clsH, midMain, NULL); //Calling the main method.
		
	//Release resources.
	int n = jvm->DestroyJavaVM();
    return 0;
}
