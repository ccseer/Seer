# Seer 
#####A Windows quick look tool

## Usage

![usage](https://raw.githubusercontent.com/ccseer/Seer/master/img/md.jpg)




## System Requirements ![](https://raw.githubusercontent.com/ccseer/Seer/master/img/windows-logo.png)

**Supported Operating System**
Windows Vista,Windows 7,Windows 8,Windows 10

## Supported Type

- Audio & Video
- Text
- Compressed
- Image
- Markdown
- PDF
- Folder
	

## Download

- [SourceForge](https://sourceforge.net/projects/ccseer/)

## 3rd party explorer

- [x] [Directory Opus](https://www.gpsoft.com.au/)
		Usage:  Settings -> Explorer -> Directory Opus -> Set dopusrt.exe path – Ok
      
- [x] [Clover](http://ejie.me/)
      
- [x] [QTTabBar](https://sourceforge.net/projects/qttabbar/)
      
- [ ] [Total Commander](http://www.ghisler.com/)  **（Note：If anyone knows how to obtain the currently selected file from Total Commander, please give me a hand, email me and thank you in advance.）**
      

## Plugin
####How to add a plugin
- The extension of the file is not case sensitive.
- Drag the line number to change sequence
- Triggered only check the plug-in type at a time

![plugin](https://raw.githubusercontent.com/ccseer/Seer/master/img/plugins-add.jpg)

####How to write a plugin
Seer will add two strings after command line when invoking the plugin. 
1. 	The front string is the full path to the triggered file, for example, "E:/my_folder/name.ttf".
2. 	The other string has a full path of the file and there is no file suffix. For example, "C:/Users/COREY/AppData/Local/Temp/Seer/a-random-name". 

The plugin will analysis triggered file into the format of one of the built-in Seer suffix, name the resulting file as a-random-name, and save it on the path, “C:/Users/COREY/AppData/Local/Temp/Seer/”.

To illustrate the function, we use the mentioned font file above as an example.
The invocation of Seer is shown as below. 
`C:/Seer/fontpreview.exe C:/triggering-path/FILE.TTF C:/Seer-temp-path/ABCabc123`
**fontpreview.exe would render FILE.TTF to ABCabc123.png**

*Source Code*
- fontpreview [Github](https://github.com/ccseer/Seer-plugins/blob/master/font/fontpreview_py.py)
- MS-Office  [Github](https://github.com/ccseer/Seer-plugins/blob/master/ms-office/1syt.py)


Contact: cc.seer@gmail.com



* * *


# Seer 帮助文档 

## 使用方法

![usage](https://raw.githubusercontent.com/ccseer/Seer/master/img/md.jpg)




## 系统支持 ![](https://raw.githubusercontent.com/ccseer/Seer/master/img/windows-logo.png)

最低支持 — Windows Vista

## 当前支持内容

- 音频、视频
- 文本
- 压缩包
- 图片
- Markdown
- PDF
- 文件夹 
	

## 下载地址

- [SourceForge](https://sourceforge.net/projects/ccseer/)

## 第三方资管管理器

- [x] [Directory Opus](https://www.gpsoft.com.au/)
		开启方法: 设置 - 资源管理器 - Directory Opus - 设置 dopusrt.exe - 确定
      
- [x] [Clover](http://ejie.me/)
      
- [x] [QTTabBar](https://sourceforge.net/projects/qttabbar/)
      
- [ ] [Total Commander](http://www.ghisler.com/)  **（如果有人知道如何从TC中获取当前选中文件，希望能够帮助我。）**
      
      需要支持更多第三方资源管理器？给我发邮件吧。

## 插件
####添加
- 文件后缀名大小写不敏感
- 可以通过拖动序号改变顺序
- 每次触发仅检查一次插件类型

![plugin](https://raw.githubusercontent.com/ccseer/Seer/master/img/plugins-add.jpg)

####编写
Seer 调用插件时会在调用命令最后追加两个字符串，前一个为用户当前触发文件的完整路径（例如:"E:/my_folder/name.ttf"），最后一个为插件转换后的保存路径，仅带有完整的文件路径，没有后缀名（例如："C:/Users/COREY/AppData/Local/Temp/Seer/A-Random-Name"）。插件需要将触发文件解析成 Seer 内置格式的其中一种后缀，并将解析生成的文件命名为 A-Random-Name，并保存到 C:/Users/COREY/AppData/Local/Temp/Seer/ 下。

以上图的字体文件为例：Seer 调用方式为 :
	`C:/Seer/fontpreview.exe C:/触发路径/触发文件.ttf C:/临时文件夹路径/随机文件名`
C:/Seer/fontpreview.exe 会把 触发文件.ttf 渲染成图片，保存为 C:/临时文件夹路径/随机文件名.png

- fontpreview [Github](https://github.com/ccseer/Seer-plugins/blob/master/font/fontpreview_py.py)
- MS-Office  [Github](https://github.com/ccseer/Seer-plugins/blob/master/ms-office/1syt.py)

联系方式: cc.seer@gmail.com

    
    
      
	 
    
    


















