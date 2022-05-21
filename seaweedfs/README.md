This is a example of [SeaweedFS](https://github.com/chrislusf/seaweedfs) golang app running on OSv.

Here are the examples on how to build and run it:
* master
```
./scripts/build image=seaweedfs
./scripts/run.py --forward 'tcp::9333-:9333'
```

* server
```
./scripts/build image=seaweedfs.server
./scripts/run.py --forward 'tcp::9333-:9333'
```
