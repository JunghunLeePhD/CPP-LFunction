let chartLog = null;
let subCharts = []; // Store instances to destroy them properly

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

        // Prepare datasets with colors
        const datasets = data.datasets.map((ds, index) => ({
            label: `Chi ${ds.label}`,
            data: ds.data,
            borderColor: getColor(index, data.datasets.length),
            backgroundColor: 'transparent',
            borderWidth: 1.5,
            pointRadius: 0,
            tension: 0.2,
        }));

        // --- 1. Render Main Chart ---
        if (chartLog) chartLog.destroy();
        const ctx = document.getElementById('logChart').getContext('2d');
        chartLog = new Chart(ctx, {
            type: 'line',
            data: { labels: labels, datasets: datasets },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                interaction: { mode: 'index', intersect: false },
                plugins: {
                    title: { display: true, text: `Combined: log|L(0.5+it)| for mod ${q}` },
                },
                scales: {
                    x: { title: { display: true, text: 't' }, ticks: { maxTicksLimit: 20 } },
                    y: { title: { display: true, text: 'log |L|' } },
                },
                animation: false,
            },
        });

        // --- 2. Render Sub-Charts ---
        const grid = document.getElementById('sub-charts-grid');

        // Cleanup old charts
        subCharts.forEach((c) => c.destroy());
        subCharts = [];
        grid.innerHTML = '';

        // Loop through each dataset and create a small card
        datasets.forEach((ds, index) => {
            // Create DOM elements
            const card = document.createElement('div');
            card.className = 'sub-chart-card';

            const title = document.createElement('div');
            title.className = 'sub-chart-title';
            title.innerText = `Character Index ${ds.label.replace('Chi ', '')}`;

            const canvasContainer = document.createElement('div');
            canvasContainer.style.height = '200px'; // Smaller height for sub-figures
            canvasContainer.style.position = 'relative';

            const canvas = document.createElement('canvas');
            canvasContainer.appendChild(canvas);
            card.appendChild(title);
            card.appendChild(canvasContainer);
            grid.appendChild(card);

            // Create Chart Instance
            const newChart = new Chart(canvas.getContext('2d'), {
                type: 'line',
                data: {
                    labels: labels,
                    datasets: [ds], // Only this specific dataset
                },
                options: {
                    responsive: true,
                    maintainAspectRatio: false,
                    animation: false,
                    plugins: { legend: { display: false } }, // Hide legend, title is enough
                    scales: {
                        x: { display: false }, // Hide x-axis labels to save space
                        y: { title: { display: true, text: 'log |L|' } },
                    },
                },
            });
            subCharts.push(newChart);
        });
    } catch (error) {
        console.error('Error computing:', error);
    } finally {
        if (loader) loader.style.display = 'none';
    }
}

document.addEventListener('DOMContentLoaded', updateCharts);
