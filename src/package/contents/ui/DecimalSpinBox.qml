/*
 * SPDX-FileCopyrightText: 2021 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick
import QtQuick.Controls as QtControls2

Item {
    // QtControls 2 doen't provide a decimal spinbox out of the box. So we wrap the integer
    // based spinbox and perform integer <-> decimal conversions internally.

    id: container
    implicitWidth: control.implicitWidth
    implicitHeight: control.implicitHeight

    function __integerToDecimal(integer) {
        return integer / 100;
    }
    function __decimalToInteger(decimal) {
        return Math.round(decimal * 100);
    }

    property real from: 0
    property real to: 99

    property int decimals: 2
    property real value: 0
    property real stepSize: 1

    QtControls2.SpinBox {
        id: control
        anchors.fill: parent

        from: __decimalToInteger(container.from)
        to: __decimalToInteger(container.to)
        stepSize: __decimalToInteger(container.stepSize)
        value: __decimalToInteger(container.value)

        validator: DoubleValidator {
            bottom: Math.min(control.from, control.to)
            top:  Math.max(control.from, control.to)
        }

        textFromValue: function(value, locale) {
            return __integerToDecimal(value).toLocaleString(locale, 'f', container.decimals);
        }
        valueFromText: function(text, locale) {
            return __decimalToInteger(Number.fromLocaleString(locale, text));
        }

        onValueModified: {
            // Update the outer decimal value, this will be called when user has modified the
            // spinbox value, so it should not be possible to have a binding loop.
            container.value = __integerToDecimal(control.value);
        }
    }
}
