package dev.glist.android.lib; // Do not change! GlistEngine links to this package.

import android.content.Context;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.widget.EditText;

/**
 * A tiny, invisible text field that exists only to receive soft-keyboard input on
 * behalf of the engine's GUI text controls. The user never touches it: when a
 * gGUITextbox gains edit focus the engine calls GlistNative.showKeyboard(), which
 * focuses this view and raises the IME; what the keyboard produces is forwarded to
 * native (onCharTyped / onKey) and on to the focused control.
 *
 * Routing input through a real EditText - rather than making the GL SurfaceView
 * pretend to be an editor - is what keeps the IME behaving normally: a plain
 * keyboard, instead of the voice/selection/assist menus a non-editor surface
 * provokes. The buffer it keeps is invisible and cleared each time the keyboard
 * hides.
 */
public class GlistInputView extends EditText {

    // Engine key codes (GLFW-based, see engine/utils/gKeyCode.h).
    private static final int G_KEY_ENTER = 257;
    private static final int G_KEY_BACKSPACE = 259;
    private static final int G_KEY_LEFT = 263;
    private static final int G_KEY_RIGHT = 262;

    public GlistInputView(Context context) {
        super(context);
        setFocusable(true);
        setFocusableInTouchMode(true);
        setCursorVisible(false);
        // A plain keyboard with no suggestion strip: the engine owns the text, so
        // autocorrect/suggestions here would only get in the way.
        setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
        // The user never touches this view, but make sure it can never raise a
        // text-selection toolbar even if it somehow received a tap.
        setLongClickable(false);
        setTextIsSelectable(false);
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        outAttrs.inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
        outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE
                | EditorInfo.IME_FLAG_NO_EXTRACT_UI
                | EditorInfo.IME_FLAG_NO_FULLSCREEN;
        return new GlistInputConnection(this);
    }

    // Enter and the arrows are forwarded as key events; backspace is handled in the
    // connection's deleteSurroundingText so it is never counted twice.
    private static int mapKeyCode(int androidKeyCode) {
        switch (androidKeyCode) {
            case KeyEvent.KEYCODE_ENTER: return G_KEY_ENTER;
            case KeyEvent.KEYCODE_NUMPAD_ENTER: return G_KEY_ENTER;
            case KeyEvent.KEYCODE_DPAD_LEFT: return G_KEY_LEFT;
            case KeyEvent.KEYCODE_DPAD_RIGHT: return G_KEY_RIGHT;
            default: return -1;
        }
    }

    private static class GlistInputConnection extends BaseInputConnection {

        GlistInputConnection(GlistInputView targetView) {
            // fullEditor = true keeps a real editable buffer, so the IME's own state
            // (composing region, cursor) stays consistent and backspace arrives as
            // deleteSurroundingText rather than a key event.
            super(targetView, true);
        }

        @Override
        public boolean commitText(CharSequence text, int newCursorPosition) {
            if (text != null) {
                for (int i = 0; i < text.length(); ) {
                    int codePoint = Character.codePointAt(text, i);
                    GlistNative.onCharTyped(codePoint);
                    i += Character.charCount(codePoint);
                }
            }
            return super.commitText(text, newCursorPosition);
        }

        @Override
        public boolean deleteSurroundingText(int beforeLength, int afterLength) {
            // Each character the keyboard deletes becomes one backspace the engine
            // knows. The single path (never also sendKeyEvent) avoids double delete.
            for (int i = 0; i < beforeLength; i++) {
                GlistNative.onKey(G_KEY_BACKSPACE, true);
                GlistNative.onKey(G_KEY_BACKSPACE, false);
            }
            return super.deleteSurroundingText(beforeLength, afterLength);
        }

        @Override
        public boolean sendKeyEvent(KeyEvent event) {
            int engineKey = mapKeyCode(event.getKeyCode());
            if (engineKey != -1) {
                if (event.getAction() == KeyEvent.ACTION_DOWN) {
                    GlistNative.onKey(engineKey, true);
                } else if (event.getAction() == KeyEvent.ACTION_UP) {
                    GlistNative.onKey(engineKey, false);
                }
            } else if (event.getAction() == KeyEvent.ACTION_DOWN) {
                // A hardware keyboard delivers characters as key events; relay those.
                int unicode = event.getUnicodeChar();
                if (unicode != 0) {
                    GlistNative.onCharTyped(unicode);
                }
            }
            return super.sendKeyEvent(event);
        }
    }
}
