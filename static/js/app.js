let chartLog = null;

function getColor(index, total) {
    const hue = (index * 360) / total;
    return `hsla(${hue}, 70%, 50%, 0.8)`;
}

async function updateCharts() {
    const start = document.getElementById('start').value;
    const end = document.getElementById('end').value;
    const q = document.getElementById('mod').value;

    const loader = document.getElementById('loading-sign');
    if (loader) loader.style.display = 'block';

    try {
        const response = await fetch(`/scan_all?start=${start}&end=${end}&q=${q}`);
        const data = await response.json();

        if (data.error) {
            console.error(data.error);
            return;
        }

        const labels = data.t.map((t) => t.toFixed(2));
        const datasets = data.datasets.map((ds, index) => ({
            label: `Chi ${ds.label}`, // Label like "Chi 1"
            data: ds.data,
            borderColor: getColor(index, data.datasets.length),
            backgroundColor: 'transparent',
            borderWidth: 1.5,
            pointRadius: 0,
            tension: 0.2, // slight curve
        }));

        if (chartLog) {
            chartLog.destroy();
        }

        const ctx = document.getElementById('logChart').getContext('2d');
        chartLog = new Chart(ctx, {
            type: 'line',
            data: {
                labels: labels,
                datasets: datasets,
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                interaction: {
                    mode: 'index',
                    intersect: false,
                },
                plugins: {
                    title: {
                        display: true,
                        text: `log|L(0.5+it)| for all characters mod ${q}`,
                    },
                    tooltip: {
                        callbacks: {
                            label: function (context) {
                                return context.dataset.label + ': ' + context.parsed.y.toFixed(3);
                            },
                        },
                    },
                },
                scales: {
                    x: {
                        title: { display: true, text: 't' },
                        ticks: { maxTicksLimit: 20 },
                    },
                    y: {
                        title: { display: true, text: 'log |L|' },
                    },
                },
                animation: false,
            },
        });
    } catch (error) {
        console.error('Error computing:', error);
    } finally {
        if (loader) loader.style.display = 'none';
    }
}

document.addEventListener('DOMContentLoaded', updateCharts);
