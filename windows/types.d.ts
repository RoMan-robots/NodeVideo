declare module './addons/screen_capture.node' {
    export interface ScreenCapture {
        start(callback: (frame: any) => void): void;
        stop(): void;
    }
    const screen: ScreenCapture;
    export default screen;
}