# stay-in-bed
Wake up light to tell my daughter whether it's time to get up or stay in bed.

## Setup
### Linux
- Copy `tools/Linux/udev/70-st-link.rules` to `/etc/udev/` and run `udevadm control --reload-rules`.

## Use
### Run Tests
```
make -f test.mk
```

### Compile
```shell
make -f target.mk
```

### Clean
```shell
make -f target.mk clean
```

### Erase via SWIM
```shell
make -f target.mk erase
```

### Flash via SWIM
```shell
make -f target.mk upload
```
