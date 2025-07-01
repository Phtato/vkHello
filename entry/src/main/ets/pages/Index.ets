import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

const DOMAIN = 0x0000;

class MyXComponentController extends XComponentController {
  onSurfaceCreated(surfaceId: string): void {
    console.log(`onSurfaceCreated surfaceId: ${surfaceId}`)
    testNapi.SetSurfaceId(BigInt(surfaceId));
  }

  onSurfaceChanged(surfaceId: string, rect: SurfaceRect): void {
    console.log(`onSurfaceChanged surfaceId: ${surfaceId}, rect: ${JSON.stringify(rect)}}`)
    // 在onSurfaceChanged中调用ChangeSurface绘制内容
    testNapi.ChangeSurface(BigInt(surfaceId), rect.surfaceWidth, rect.surfaceHeight)
  }

  onSurfaceDestroyed(surfaceId: string): void {
    console.log(`onSurfaceDestroyed surfaceId: ${surfaceId}`)
    testNapi.DestroySurface(BigInt(surfaceId))
  }
}

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  xComponentController: XComponentController = new MyXComponentController();
  @State currentStatus: string = "index";

  build() {
    Row() {
      Column() {
        Column({space: 10}){
          XComponent({
            type:XComponentType.SURFACE,
            controller: this.xComponentController
          })
          Text(this.currentStatus)
            .fontSize('24fp')
            .fontWeight(500)
        }            
        .onClick(() => {
          let surfaceId = this.xComponentController.getXComponentSurfaceId()
          testNapi.ChangeColor(BigInt(surfaceId))
          let hasChangeColor: boolean = false;
          if (testNapi.GetXComponentStatus(BigInt(surfaceId))) {
            hasChangeColor = testNapi.GetXComponentStatus(BigInt(surfaceId)).hasChangeColor;
          }
          if (hasChangeColor) {
            this.currentStatus = "change color";
          }
        })
        Row() {
          Button('Draw Star')
            .fontSize('16fp')
            .fontWeight(500)
            .margin({ bottom: 24 })
            .onClick(() => {
              let surfaceId = this.xComponentController.getXComponentSurfaceId()
              testNapi.DrawPattern(BigInt(surfaceId))
              let hasDraw: boolean = false;
              if (testNapi.GetXComponentStatus(BigInt(surfaceId))) {
                hasDraw = testNapi.GetXComponentStatus(BigInt(surfaceId)).hasDraw;
              }
              if (hasDraw) {
                this.currentStatus = "draw star"
              }
            })
            .width('53.6%')
            .height(40)
        }
        .width('100%')
        .justifyContent(FlexAlign.Center)
        .alignItems(VerticalAlign.Bottom)
        .layoutWeight(1)
        Text(this.message)
          .fontSize($r('app.float.page_text_font_size'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.message = 'Welcome';
            hilog.info(DOMAIN, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.run(2, 3));
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
