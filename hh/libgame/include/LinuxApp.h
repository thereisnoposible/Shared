#ifndef LINUXAPP_H
#define LINUXAPP_H

#include "BaseApp.h"

class LinuxApp : public BaseApp
{
public:
    LinuxApp(xstring appname,HINSTANCE pInstance);
    virtual ~LinuxApp();

public:
     /**
     * @brief		在屏幕上打印信息
     * @details		纯虚函数，派生类实现
     * @return		void
     * @remarks
    */
     virtual void PrintMessage(MessageLevel level, const char* message, ...);


     /**
     * \fn				SetStatus
     * @brief		设置状态栏显示
     * @param[in]	message[xstring]  : 显示内容
     * @return		  void
     * @remarks
    */
     virtual void SetStatus(const xstring message);

     virtual void Run();

     static void onSigInt(int sig);

     void Quit();

protected:
      /**
      * @brief		初始化工作
      * @details		初始化所需对象，纯虚函数
      * @return		bool
      * @remarks
     */
      virtual bool  initialize();


       /**
       * @brief		销毁所有资源
       * @details		纯虚函数
       * @remarks
      */
       virtual void destroy();



private:
        bool _bQuit;
};

#endif // LINUXAPP_H
