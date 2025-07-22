import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';
import { common } from '@kit.AbilityKit';
import { taskpool } from '@kit.ArkTS';
import fs from '@ohos.file.fs';

/*class MyXComponentController extends XComponentController {
  onSurfaceCreated(surfaceId: string): void {
    console.log(`onSurfaceCreated surfaceId: ${surfaceId}`)
    //testNapi.SetSurfaceId(BigInt(surfaceId));
  }

*/
/*  onSurfaceChanged(surfaceId: string, rect: SurfaceRect): void {
    console.log(`onSurfaceChanged surfaceId: ${surfaceId}, rect: ${JSON.stringify(rect)}}`)
    // 在onSurfaceChanged中调用ChangeSurface绘制内容
    testNapi.ChangeSurface(BigInt(surfaceId), rect.surfaceWidth, rect.surfaceHeight)
  }

  onSurfaceDestroyed(surfaceId: string): void {
    console.log(`onSurfaceDestroyed surfaceId: ${surfaceId}`)
    testNapi.DestroySurface(BigInt(surfaceId))
  }*/ /*
}*/

@Concurrent
function render(context : common.UIAbilityContext){
  testNapi.run(context.resourceManager);
}

@Entry
@ComponentV2
struct Index {
  context = this.getUIContext().getHostContext() as common.UIAbilityContext;
  @Local buttonStatus: boolean = true;

  aboutToAppear(): void {
/*    // Copy squid.json to hap sandbox
    let path = getContext(this).filesDir;
    if (!fs.accessSync(path)) {
      fs.mkdirSync(path);
    }

    let buffer = getContext(this).resourceManager.getRawFileContentSync('squid.json');
    let file = fs.openSync(path + '/squid.json', fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    fs.writeSync(file.fd, buffer.buffer);*/
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
          .onAppear(()=>{
            console.error("tujiaming");
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
          taskpool.execute(render,this.context);
          this.buttonStatus = false;
        })
        .enabled(this.buttonStatus)

      }
      .width('100%')
    }
    .height('100%')
  }
}
