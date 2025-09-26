import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';
import { common } from '@kit.AbilityKit';
import { taskpool } from '@kit.ArkTS';
import fs from '@ohos.file.fs';
import { BusinessError } from '@kit.BasicServicesKit';
import fileIo from '@ohos.file.fs';

@Concurrent
function render(context: common.UIAbilityContext) {
  //prepare
  let sandboxPath = context.filesDir;
  console.log("tujiaming", `${sandboxPath}`);
  testNapi.transferSandboxPath(sandboxPath);

  testNapi.run(context.resourceManager);
}

@Concurrent
function prepare(context: common.UIAbilityContext) {
  //prepare
  let sandboxPath = context.filesDir;
  testNapi.transferSandboxPath(sandboxPath);
}

@Entry
@ComponentV2
struct Index {
  context = this.getUIContext().getHostContext() as common.UIAbilityContext;
  @Local buttonStatus: boolean = true;

  aboutToAppear(): void {
    // Copy squid.json to hap sandbox
    let path = getContext(this).filesDir;
    if (!fs.accessSync(path)) {
      fs.mkdirSync(path);
    }

    let buffer = getContext(this).resourceManager.getRawFileContentSync('squid.json');
    let file = fs.openSync(path + '/squid.json', fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    fs.writeSync(file.fd, buffer.buffer);
  }

  build() {
    Row() {
      Column() {
        XComponent({ id: 'xcomponentId', type: XComponentType.SURFACE, libraryname: 'entry' })
          .width(360)
          .height(360)
          .onAttach(() => {
            //testNapi.run(this.context.resourceManager);
          })
          .onAppear(() => {
            testNapi.run(this.context.resourceManager);
          })
        Button() {
          Text("加载")
            .fontSize(20)
            .fontColor(Color.White)
        }
        .width(180)
        .height(50)
        .margin({
          top: 20
        })
        .onClick(async () => {
        })
        .enabled(this.buttonStatus)
        .onAppear(() => {
          this.buttonStatus = false;
          let sandboxPath = this.context.filesDir;
          testNapi.transferSandboxPath(sandboxPath);
          taskpool.execute(render, this.context);
        })

      }
      .width('100%')
    }
    .height('100%')
  }
}
