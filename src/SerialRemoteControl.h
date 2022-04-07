
#include <Arduino.h>
#include <SerialCommands.h>

namespace SerialRemoteControl 
{
    namespace // private
    {
        char serial_command_buffer_[64];
    

        void cmd_unrecognized(SerialCommands* sender, const char* cmd)
        {
            sender->GetSerial()->printf("log ERROR: Unrecognized command [%s]\r\n",cmd);
        }

        SerialCommands *serialCommands;
    }

    void Init()
    {
        serialCommands = new SerialCommands(&Serial, serial_command_buffer_, sizeof(serial_command_buffer_), "\n", " ");
        serialCommands->SetDefaultHandler(&cmd_unrecognized);
    }
    void ReadSerial()
    {
        serialCommands->ReadSerial();
    }

    namespace // private
    { 
        void(*cmd_setOutTarget_cb)(int);
        int cmd_setOutTarget_maxVal;
        int cmd_setOutTarget_minVal;
        void cmd_setOutTarget(SerialCommands* sender)
        {
            char* target_id_str = sender->Next();
            if (target_id_str == NULL)
            {
                sender->GetSerial()->println("log ERROR_NO_TARGET_ID");
                return;
            }
            int target_id = atoi(target_id_str);
            if (target_id >= cmd_setOutTarget_maxVal || target_id <= cmd_setOutTarget_minVal)
            {
                sender->GetSerial()->printf("log ERROR_UNKNOWN_TARGET_ID [%d]\r\n", target_id);
                return;
            }
            if (cmd_setOutTarget_cb)
                cmd_setOutTarget_cb(target_id);
        }
    }

    void SetCB_cmd_setOutTarget(const char *cmd, void(*func)(int), int minVal, int maxVal)
    {
        cmd_setOutTarget_cb = func;
        cmd_setOutTarget_minVal = minVal;
        cmd_setOutTarget_maxVal = maxVal;
        serialCommands->AddCommand(new SerialCommand(cmd, cmd_setOutTarget));
    }

    namespace // private
    { 
        void(*cmd_setGradientColorMap_cb)(int);
        int cmd_setGradientColorMap_maxVal;
        void cmd_setGradientColorMap(SerialCommands* sender)
        {
            char* colormapIndex_str = sender->Next();
            if (colormapIndex_str == NULL)
            {
                sender->GetSerial()->printf("log ERROR_NO_COLORMAP_INDEX");
                return;
            }
            int colormapIndex = atoi(colormapIndex_str);
            if (colormapIndex < 0 || colormapIndex >= cmd_setGradientColorMap_maxVal)
            {
                sender->GetSerial()->printf("log ERROR_UNKNOWN_COLORMAP_INDEX [%d]\r\n", colormapIndex);
                return;
            }
            cmd_setGradientColorMap_cb(colormapIndex);
        }
    }


    void SetCB_cmd_setGradientColorMap(const char *cmd, void(*func)(int), uint32_t maxVal)
    {
        cmd_setGradientColorMap_cb = func;
        cmd_setGradientColorMap_maxVal = maxVal;
        serialCommands->AddCommand(new SerialCommand(cmd, cmd_setGradientColorMap));
    }

    namespace // private
    { 
        void(*cmd_setInterpolatedSize_cb)(int,int);
        int cmd_setInterpolatedSize_minWidth;
        int cmd_setInterpolatedSize_maxWidth;
        int cmd_setInterpolatedSize_minHeight;
        int cmd_setInterpolatedSize_maxHeight;
        void cmd_setInterpolatedSize(SerialCommands* sender)
        {
            char* width_str = sender->Next();
            if (width_str == NULL)
            {
                sender->GetSerial()->printf("log ERROR_NO_INTERPOLATE_SIZE");
                return;
            }
            int width = atoi(width_str);
            if (width < cmd_setInterpolatedSize_minWidth || width > cmd_setInterpolatedSize_maxWidth)
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
            if (height < cmd_setInterpolatedSize_minHeight || height > cmd_setInterpolatedSize_maxHeight)
            {
                sender->GetSerial()->printf("log ERROR_INTERPOLATE_HEIGHT [%d]\r\n", height);
                return;
            }
            cmd_setInterpolatedSize_cb(width, height);
        }
    }

    void SetCB_cmd_setInterpolatedSize(const char *cmd, void(*func)(int,int), int minWidth, int maxWidth, int minHeight, int maxHeight)
    {
        cmd_setInterpolatedSize_minWidth = minWidth;
        cmd_setInterpolatedSize_maxWidth = maxWidth;
        cmd_setInterpolatedSize_minHeight = minHeight;
        cmd_setInterpolatedSize_maxHeight = maxHeight;
        cmd_setInterpolatedSize_cb = func;
        serialCommands->AddCommand(new SerialCommand(cmd, cmd_setInterpolatedSize));
    }
};