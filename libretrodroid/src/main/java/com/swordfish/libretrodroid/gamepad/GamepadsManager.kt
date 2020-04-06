/*
 *     Copyright (C) 2019  Filippo Scognamiglio
 *
 *     This program is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

package com.swordfish.libretrodroid.gamepad

import android.content.Context
import android.hardware.input.InputManager
import android.view.InputDevice
import android.view.KeyEvent
import com.jakewharton.rxrelay2.BehaviorRelay
import io.reactivex.Observable

internal class GamepadsManager(appContext: Context): InputManager.InputDeviceListener {
    private val devicePortMap: MutableMap<Int, Int> = mutableMapOf()
    private val inputManager = appContext.getSystemService(Context.INPUT_SERVICE) as InputManager

    private val infosSubject = BehaviorRelay.createDefault<List<GamepadInfo>>(listOf())

    override fun onInputDeviceAdded(p0: Int) = updateDevicePortMap()

    override fun onInputDeviceChanged(p0: Int) = updateDevicePortMap()

    override fun onInputDeviceRemoved(p0: Int) = updateDevicePortMap()

    fun init() {
        inputManager.registerInputDeviceListener(this, null)
        updateDevicePortMap()
    }

    fun deinit() {
        inputManager.unregisterInputDeviceListener(this)
    }

    /** The Android gamepad layout is different from RetroPad since X/Y and A/B buttons are inverted. */
    fun getGamepadKeyEvent(keyCode: Int): Int {
        return when (keyCode) {
            KeyEvent.KEYCODE_BUTTON_B -> KeyEvent.KEYCODE_BUTTON_A
            KeyEvent.KEYCODE_BUTTON_A -> KeyEvent.KEYCODE_BUTTON_B
            KeyEvent.KEYCODE_BUTTON_X -> KeyEvent.KEYCODE_BUTTON_Y
            KeyEvent.KEYCODE_BUTTON_Y -> KeyEvent.KEYCODE_BUTTON_X
            else -> keyCode
        }
    }

    fun getGamepadPort(deviceId: Int): Int {
        return devicePortMap.getOrElse(deviceId) { 0 }
    }

    fun getGamepadInfos(): Observable<List<GamepadInfo>> {
        return infosSubject.distinctUntilChanged()
    }

    private fun updateDevicePortMap() {
        devicePortMap.clear()

        InputDevice.getDeviceIds()
            .map { InputDevice.getDevice(it) }
            .filter { isGamePad(it) }
            .filter { it.controllerNumber > 0 }
            .forEach { devicePortMap[it.id] = it.controllerNumber - 1 }

        infosSubject.accept(buildGamepadInfos())
    }

    private fun buildGamepadInfos(): List<GamepadInfo> {
        return devicePortMap.entries
            .sortedBy { (_, value) -> value }
            .map { (key, _) -> InputDevice.getDevice(key) }
            .map { device -> GamepadInfo.GAMEPAD_KEYS.filter { device.hasKeys(it)[0] } }
            .map { GamepadInfo(it.toSet()) }
    }

    private fun isGamePad(it: InputDevice): Boolean {
        val conditions = listOf(
                it.sources and InputDevice.SOURCE_GAMEPAD == InputDevice.SOURCE_GAMEPAD,
                it.sources and InputDevice.SOURCE_JOYSTICK == InputDevice.SOURCE_JOYSTICK
        )

        return conditions.any()
    }
}