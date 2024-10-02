### Set git

```
git clone <lvgl-9.2-demo>
cd lvgl-9.2-demo
git clone --branch release/v9.2 https://github.com/lvgl/lvgl.git
```

### Set only one of the below as 1 in `lv_conf.h`:

```
#define LV_USE_LINUX_FBDEV	0
#define LV_USE_LINUX_DRM	1
#define LV_USE_SDL		0
```

### Run

```
[ -d "build" ] && rm -rf build && mkdir build
cmake -B build -S . && cmake --build build -j
./build/bin/main
```
