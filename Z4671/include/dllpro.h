// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 DLLPRO_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// DLLPRO_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef DLLPRO_EXPORTS
#define DLLPRO_API __declspec(dllexport)
#else
#define DLLPRO_API __declspec(dllimport)
#endif
#define DllImport __declspec(dllimport)

// 此类是从 dllpro.dll 导出的
class DLLPRO_API Cdllpro {
public:
	Cdllpro(void);
	// TODO: 在此添加您的方法。
};

extern DLLPRO_API int ndllpro;

//DLLPRO_API int fndllpro(void);

//DLLPRO_API int fndllprolee(void);

DllImport bool UploadXMLMessage(char * pszXML,char* pszRes);
//DLLPRO_API bool UploadXMLMessage(BSTR bstr_XML,char* pszRes);
