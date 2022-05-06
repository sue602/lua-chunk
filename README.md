# lua-chunk 从zip压缩包加载文件
### 1.依赖 libzip , https://libzip.org/
### 2.在skynet内测试代码
    ``` lua
    local skynet = require "skynet"
    skynet.sleep(100)
    local chunk = require("chunk")
    local success = chunk.loadzip("testzip.zip","222222")

    require("testservice")
    Log.i("success ==",success)
    skynet.sleep(200)
    os.exit()
    ```