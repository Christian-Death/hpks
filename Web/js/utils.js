
/*
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 * 
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 * File:   index.html
 * Author: Christian Rost  <rost at entronix.de>
 *
 * Created on 11. Februar 2015, 11:04
 */



function dynamicSort(property) {
    var sortOrder = 1;
    if (property[0] === "-") {
        sortOrder = -1;
        property = property.substr(1);
    }
    return function (a, b) {
        var result = (a[property] < b[property]) ? -1 : (a[property] > b[property]) ? 1 : 0;
        return result * sortOrder;
    };
}

function dynamicSortMultiple() {
    /*
     * save the arguments object as it will be overwritten
     * note that arguments object is an array-like object
     * consisting of the names of the properties to sort by
     */
    var props = arguments;
    return function (obj1, obj2) {
        var i = 0, result = 0, numberOfProperties = props.length;
        /* try getting a different result from 0 (equal)
         * as long as we have extra properties to compare
         */
        while (result === 0 && i < numberOfProperties) {
            result = dynamicSort(props[i])(obj1, obj2);
            i++;
        }
        return result;
    };
}

function ShowInfoDlg(message) {
    $("#msg_dialog p").text(message);
    $("#msg_dialog").dialog(
            "option",
            "buttons", [
                {
                    text: "Ok",
                    click: function () {
                        $(this).dialog('close');
                    }
                }]);
    $("#msg_dialog").dialog("open");
    setTimeout(function () {
        $('#msg_dialog').dialog('close');
    }, 5000);
}

function InfoDlg(data) {
    $.each(data, function (i, item) {
        //$('#' + i).html(item);
        if (i === 'msg') {
            ShowInfoDlg(item);
        }
    });
}