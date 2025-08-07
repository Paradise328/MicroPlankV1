# MicroSurgery Robot

# project structure

you must construct the project as following structure:

```
mikroPlack root folder
|- build
|- config(用于放置log的配置文件、 EtherCAT的PDO文件、 机器人的参数文件)
|- log(日志文件)
|- Modules(模块)
    |- LoggerModule(日志打印模块)
    |- MasterModule(主手模块)
    |- MathModule(数学模块，包含一些计算函数)
    |- MotorDriverModule(EtherCAT通信模块)
    |- MsgModule(消息队列模块)
    |- PeripheralDeviceModule(外部设备(422、232等库))
    |- SecurityModule(系统安全模块)
    |- SystemUtilsModule(系统不同模块都需要调用的参数、结构体等)
|- gitignore
|- README.md
```

# instructions


## some commands

to start cifx card:
sudo modprobe uio_netx

to start com card:
sudo emp2init

## build

## run:

# Update Log

# Todo List
修改路径：
1. 在 main.cpp 中修改 log config 的路径
2. 在 motorDriver.h 中修改 m _mappingPath
3. 在 robotControl.h 中修改 m_configFilePath 以及 m_robotConfigPath


