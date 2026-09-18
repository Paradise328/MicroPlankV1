import QtQuick 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: chart
    property string title: ""
    property int channel: 0
    property color lineColor: "#5BD7BD"
    property real rangeStart: 0
    property real rangeEnd: 300
    property var points: []
    color: "#15232D"
    radius: 10
    border.color: "#2A3B46"
    function refresh() {
        refreshTimer.restart()
    }
    Timer { id: refreshTimer; interval: 0; onTriggered: chart.updatePlot() }
    function updatePlot() {
        if (!visible || width < 1) return
        points = UIinterface.forcePlot(channel, rangeStart, rangeEnd, Math.max(1, Math.floor(width - 76)))
        plot.requestPaint()
    }
    function timeLabel(t) {
        t = Math.max(0, Math.floor(t))
        return Math.floor(t / 3600) + ":" + ("0" + Math.floor(t / 60) % 60).slice(-2)
            + ":" + ("0" + t % 60).slice(-2)
    }
    onRangeStartChanged: refresh()
    onRangeEndChanged: refresh()
    onWidthChanged: refresh()
    onVisibleChanged: refresh()
    Component.onCompleted: refresh()
    Connections {
        target: UIinterface
        function onForceDataChanged() { chart.refresh() }
        function onForceReset() { chart.refresh() }
    }
    RowLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 12
        Text { text: chart.title; color: "#D6E1E8"; font.pixelSize: 15 }
        Item { Layout.fillWidth: true }
        Text {
            text: UIinterface.forceValues.length === 3 ? Number(UIinterface.forceValues[chart.channel]).toFixed(3) : "—"
            color: chart.lineColor
            font.pixelSize: 19
        }
    }
    Canvas {
        id: plot
        objectName: "forceCanvas" + chart.channel
        anchors.fill: parent
        anchors.topMargin: 40
        anchors.bottomMargin: 5
        onHeightChanged: requestPaint()
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var left = 62, right = width - 14, top = 7, bottom = height - 22
            if (right <= left || bottom <= top) return
            var low = 0, high = 0, found = false
            for (var i = 0; i < chart.points.length; ++i) {
                var p = chart.points[i]
                if (!p[2]) continue
                low = Math.min(low, p[1]); high = Math.max(high, p[1]); found = true
            }
            var margin = Math.max(0.1, (high - low) * 0.1)
            low -= margin; high += margin
            ctx.font = "11px sans-serif"
            ctx.textAlign = "right"
            for (var tick = 0; tick < 3; ++tick) {
                var y = top + (bottom - top) * tick / 2
                ctx.strokeStyle = "#2A3B46"; ctx.lineWidth = 1
                ctx.beginPath(); ctx.moveTo(left, y); ctx.lineTo(right, y); ctx.stroke()
                ctx.fillStyle = "#93A8B6"
                ctx.fillText((high - (high - low) * tick / 2).toFixed(2), left - 6, y + 4)
            }
            ctx.textAlign = "left"
            ctx.fillText(chart.timeLabel(chart.rangeStart), left, height - 5)
            ctx.textAlign = "right"
            ctx.fillText(chart.timeLabel(chart.rangeEnd), right, height - 5)
            ctx.save()
            ctx.beginPath(); ctx.rect(left, top, right - left, bottom - top); ctx.clip()
            ctx.strokeStyle = chart.lineColor; ctx.fillStyle = chart.lineColor; ctx.lineWidth = 1.7
            ctx.beginPath()
            var connected = false
            for (i = 0; i < chart.points.length; ++i) {
                p = chart.points[i]
                if (!p[2]) { connected = false; continue }
                var x = left + (p[0] - chart.rangeStart) / Math.max(0.001, chart.rangeEnd - chart.rangeStart) * (right - left)
                y = bottom - (p[1] - low) / (high - low) * (bottom - top)
                if (connected) ctx.lineTo(x, y)
                else ctx.moveTo(x, y)
                connected = true
            }
            ctx.stroke()
            ctx.restore()
            if (!found) {
                ctx.textAlign = "center"; ctx.fillStyle = "#93A8B6"
                ctx.fillText("暂无有效数据", (left + right) / 2, (top + bottom) / 2)
            }
        }
    }
}
