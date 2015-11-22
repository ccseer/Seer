# Seer 帮助文档 

## 使用方法

![usage](https://raw.githubusercontent.com/ccseer/Seer/master/img/md.jpg)




## 系统支持 ![](https://raw.githubusercontent.com/ccseer/Seer/master/img/windows-logo.jpg)

最低支持 — Windows Vista

## 当前支持内容

- 音频、视频
- 文本
- 压缩包
- 图片
- Markdown
- PDF
- 文件夹
- MS Office*
		MS Office 预览默认不开启。需要本地安装 2007 及以上的 MS Office 版本。
        开启方法：Settings - Type - Other - Support MS Office Files
	

## 下载地址

- [SourceForge](https://sourceforge.net/projects/ccseer/)

## 第三方资管管理器

- [x] [Directory Opus](https://www.gpsoft.com.au/)
		需要在手动开启
        设置方法: Settings - Explorer - Directory Opus - Set dopus.exe path - Ok
      
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
Seer 调用插件时会在调用命令最后追加两个字符串,前一个为用户当前触发文件的完整路径(例如:"E:/my_folder/name.ttf"),最后一个为插件转换后的保存路径，仅带有完整的文件路径，没有后缀名（例如："C:/Users/COREY/AppData/Local/Temp/Seer/A-Random-Name"）。插件需要将触发文件解析成 Seer 内置格式的其中一种后缀，并将解析生成的文件命名为 A-Random-Name ,并保存到 C:/Users/COREY/AppData/Local/Temp/Seer/ 下。

以上图的字体文件为例：Seer 调用方式为 :
	`C:/Seer/fontpreview.exe C:/触发路径/触发文件.ttf C:/临时文件夹路径/随机文件名`
C:/Seer/fontpreview.exe 会把 触发文件.ttf 渲染成图片，保存为 C:/临时文件夹路径/随机文件名.png

fontpreview [Github](https://github.com/ccseer/Seer-plugins/blob/master/font/fontpreview_py.py)
MS-Office  [Github](https://github.com/ccseer/Seer-plugins/blob/master/ms-office/1syt.py)



    
    
      
	 
    
    


















