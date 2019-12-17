# sdl-fakeqwerty

`sdl-fakeqwerty` makes your keyboard act like a US-QWERTY keyboard, while not
changing any system settings. This is ideal for games with difficult or
impossible to change keybindings
[which get messed up when using a non-US-QWERTY keyboard][background].

This library intercepts SDL's or Xlib's event system in order to rewrite
[the `keysym`][background] to be what a US-QWERTY keyboard would return.

When activated on a _non-QWERTY_ keyboard, it means these games will have
keyboard shortcuts with the same _physical_ positions as a QWERTY keyboard. For
example:

<table>
  <tbody>
    <tr>
      <th>US-QWERTY</th>
      <th>US-Dvorak</th>
      <th>French AZERTY</th>
    </tr>
    <tr>
      <td>(default settings)</td>
      <td colspan="2"><em>(with this library)</em></td>
    </tr>
    <tr>
      <td><table><tbody style="text-align: center">
        <tr>
          <td>Q<br>Talk</td>
          <td>W<br>Move forward</td>
          <td>E<br>Use item</td>
        </tr>
        <tr>
          <td>A<br>Move left</td>
          <td>S<br>Move backward</td>
          <td>D<br>Move right</td>
        </tr>
      </tbody></table></td>
      <td><table><tbody style="text-align: center">
        <tr>
          <td>'<br>Talk</td>
          <td>,<br>Move forward</td>
          <td>.<br>Use item</td>
        </tr>
        <tr>
          <td>A<br>Move left</td>
          <td>O<br>Move backward</td>
          <td>E<br>Move right</td>
        </tr>
      </tbody></table></td>
      <td><table><tbody style="text-align: center">
        <tr>
          <td>A<br>Talk</td>
          <td>Z<br>Move forward</td>
          <td>E<br>Use item</td>
        </tr>
        <tr>
          <td>Q<br>Move left</td>
          <td>S<br>Move backward</td>
          <td>D<br>Move right</td>
        </tr>
      </tbody></table></td>
    </tr>
  </tbody>
</table>

This means you no longer have to change your keyboard layout to play a single
game, and the remapping is isolated to a single application (window) and
automated.

I've mainly tested this with some Introversion games. It might fix some other
games as well -- it has been tested with Allegro, SDL 1.2, SDL 2.0 and Unity.

Key labels shown in-game will probably be displayed _incorrectly_ as a result
of using this library (they will be the same as a US-QWERTY keyboard).

> **Note:** This will only work on Linux systems, as it relies on `LD_PRELOAD` to work.  There is limited support on OSX (Prison Architect).  I don't have interest in porting this code to other platforms.

## Documentation

* [Building this library](./building.md)
* [Games list](./games/README.md)
* [License agreement](./COPYING.md)

[background]: ./background.md
