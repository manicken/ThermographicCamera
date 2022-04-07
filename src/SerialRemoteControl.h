
#include <Arduino.h>
#include <SerialCommands.h>

namespace SerialRemoteControl 
{
    namespace serialCommands
    {
        char serial_command_buffer_[64];
        SerialCommands serialCommands(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");

        void cmd_unrecognized(SerialCommands* sender, const char* cmd)
        {
            sender->GetSerial()->printf("log ERROR: Unrecognized command [%s]\r\n",cmd);
        }  
    }

    // wrappers to make calls easier
    void Init()
    {
        serialCommands::serialCommands.SetDefaultHandler(&serialCommands::cmd_unrecognized);
    }
    void ReadSerial()
    {
        serialCommands::serialCommands.ReadSerial();
    }
    void AddCommand(const char* cmd, void(*func)(SerialCommands*), bool one_k=false)//SerialCommand* command)
    {
        serialCommands::serialCommands.AddCommand(new SerialCommand(cmd,func,one_k));
    }

    namespace cmd_setOutTarget
    { 
        void(*cb)(int);
        int minVal;
        int maxVal;
        void func(SerialCommands* sender)
        {
            char* target_id_str = sender->Next();
            if (target_id_str == NULL)
            {
                sender->GetSerial()->println("log ERROR_NO_TARGET_ID");
                return;
            }
            int target_id = atoi(target_id_str);
            if (target_id >= maxVal || target_id <= minVal)
            {
                sender->GetSerial()->printf("log ERROR_UNKNOWN_TARGET_ID [%d]\r\n", target_id);
                return;
            }
            cb(target_id);
        }
        void SetCB(const char *cmd, void(*func)(int), int minVal, int maxVal)
        {
            cmd_setOutTarget::cb = func;
            cmd_setOutTarget::minVal = minVal;
            cmd_setOutTarget::maxVal = maxVal;
            AddCommand(cmd, cmd_setOutTarget::func);
        }
    }
    
    namespace cmd_setGradientColorMap
    { 
        void(*cb)(int);
        int maxVal;
        void func(SerialCommands* sender)
        {
            char* colormapIndex_str = sender->Next();
            if (colormapIndex_str == NULL)
            {
                sender->GetSerial()->printf("log ERROR_NO_COLORMAP_INDEX");
                return;
            }
            int colormapIndex = atoi(colormapIndex_str);
            if (colormapIndex < 0 || colormapIndex >= maxVal)
            {
                sender->GetSerial()->printf("log ERROR_UNKNOWN_COLORMAP_INDEX [%d]\r\n", colormapIndex);
                return;
            }
            cb(colormapIndex);
        }
        void SetCB(const char *cmd, void(*func)(int), uint32_t maxVal)
        {
            cmd_setGradientColorMap::cb = func;
            cmd_setGradientColorMap::maxVal = maxVal;
            AddCommand(cmd, cmd_setGradientColorMap::func);
        }
    }
    
    namespace cmd_setInterpolatedSize
    { 
        void(*cb)(int,int);
        int minWidth;
        int maxWidth;
        int minHeight;
        int maxHeight;
        void func(SerialCommands* sender)
        {
            char* width_str = sender->Next();
            if (width_str == NULL)
            {
                sender->GetSerial()->printf("log ERROR_NO_INTERPOLATE_SIZE");
                return;
            }
            int width = atoi(width_str);
            if (width < minWidth || width > maxWidth)
            {
                sender->GetSerial()->printf("log ERROR_INTERPOLATE_WIDTH [%d]\r\n", width);
                return;
            }
            char* height_str = sender->Next();
            if (height_str == NULL)
            {
                sender->GetSerial()->printf("log ERROR_NO_INTERPOLATE_HEIGHT");
                return;
            }
            int height = atoi(height_str);
            if (height < minHeight || height > maxHeight)
            {
                sender->GetSerial()->printf("log ERROR_INTERPOLATE_HEIGHT [%d]\r\n", height);
                return;
            }
            cb(width, height);
        }
        void SetCB(const char *cmd, void(*func)(int,int), int minWidth, int maxWidth, int minHeight, int maxHeight)
        {
            cmd_setInterpolatedSize::minWidth = minWidth;
            cmd_setInterpolatedSize::maxWidth = maxWidth;
            cmd_setInterpolatedSize::minHeight = minHeight;
            cmd_setInterpolatedSize::maxHeight = maxHeight;
            cmd_setInterpolatedSize::cb = func;
            AddCommand(cmd, cmd_setInterpolatedSize::func);
        }
    }
};