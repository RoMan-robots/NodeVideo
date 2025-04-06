declare module 'screen_capture' {
    export interface ScreenCapture {
        startStream(callback: (frame: any) => void): void;
        stopStream(): void;
    }
    const screen: ScreenCapture;
    export default screen;
}